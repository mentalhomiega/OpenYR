/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Exercises the mod signature a joining player sends the host: what it hashes, how it is
// written and read, and how two signatures are compared. Every file it hashes is one the
// harness writes itself.

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "modsync.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-70s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


void Check_Text(std::string const & actual, std::string const & expected, char const * what)
{
	Check(actual == expected, what);

	if (actual != expected) {
		std::printf("    got [%s]\n    expected [%s]\n", actual.c_str(), expected.c_str());
	}
}


void Write_File(std::filesystem::path const & path, std::string const & content)
{
	std::filesystem::create_directories(path.parent_path());
	std::ofstream file(path, std::ios::binary | std::ios::trunc);
	file << content;
}


// A mod folder under the root, with a mod.ini naming a rules overlay.
ModClass Make_Mod(std::filesystem::path const & root, std::string const & name, std::string const & rules)
{
	std::filesystem::path const folder = root / name;
	Write_File(folder / "mod.ini", "[Mod]\nName=" + name + "\nRules=rules.ini\n");
	Write_File(folder / "rules.ini", rules);

	ModClass mod;
	mod.Folder = folder.string() + std::string(1, static_cast<char>(std::filesystem::path::preferred_separator));
	mod.Name = name;
	mod.RulesFile = (folder / "rules.ini").string();
	return(mod);
}


ModSync::Signature Sign(std::vector<ModClass> const & mods)
{
	return(ModSync::Describe(mods));
}


ModSync::Difference Differ(ModSync::Signature const & here, ModSync::Signature const & there)
{
	return(ModSync::Compare(here, there));
}

}	// namespace


int main(void)
{
	std::filesystem::path const root = std::filesystem::temp_directory_path() / "opents-modsync-test";
	std::filesystem::remove_all(root);

	ModClass const alpha = Make_Mod(root, "Alpha", "[GAPOWR]\nCost=800\n");
	ModClass const beta = Make_Mod(root, "Beta", "[HTNK]\nCost=900\n");

	/*
	 * A signature of no mods is the empty text, which is also what a build without the check
	 * sends, so the two agree.
	 */
	{
		ModSync::Signature none = Sign({});
		Check_Text(ModSync::Encode(none), "", "no mods encode to the empty text");

		ModSync::Signature read;
		Check(ModSync::Decode("", read) && read.Mods.empty() && !read.IsDigest, "the empty text reads as no mods");
		Check(Differ(none, read).Same(), "no mods match an empty field from an older build");
		Check_Text(ModSync::List_Names(none), "none", "no mods are listed as none");
	}

	/*
	 * Writing and reading give the same list back, and the same mods hash the same.
	 */
	{
		ModSync::Signature first = Sign({alpha, beta});
		ModSync::Signature second = Sign({alpha, beta});
		Check(first.Mods.size() == 2 && first.Mods[0].Name == "Alpha" && first.Mods[1].Name == "Beta", "the mods are named in order by folder");
		Check(first.Mods[0].Hash == second.Mods[0].Hash, "the same mod hashes the same twice");
		Check(first.Mods[0].Hash != first.Mods[1].Hash, "different mods hash differently");

		std::string const text = ModSync::Encode(first);
		ModSync::Signature read;
		Check(ModSync::Decode(text, read), "an encoded signature decodes");
		Check(read.Mods.size() == 2 && read.Mods[0].Name == "Alpha" && read.Mods[1].Hash == first.Mods[1].Hash, "decoding gives back the names and hashes");
		Check(Differ(first, read).Same(), "a signature matches itself after a round trip");
		Check(text.size() < ModSync::CAPACITY, "two mods fit the packet field");
	}

	/*
	 * The hash follows the contents of mod.ini and of the overlays it names, and nothing else.
	 */
	{
		std::uint32_t const before = ModSync::Hash_Mod(alpha);

		Write_File(root / "Alpha" / "rules.ini", "[GAPOWR]\nCost=801\n");
		std::uint32_t const edited = ModSync::Hash_Mod(alpha);
		Check(before != edited, "changing an overlay changes the hash");

		Write_File(root / "Alpha" / "rules.ini", "[GAPOWR]\r\nCost=801\r\n");
		Check(edited == ModSync::Hash_Mod(alpha), "line endings do not change the hash");

		Write_File(root / "Alpha" / "mod.ini", "[Mod]\nName=Other\nRules=rules.ini\n");
		Check(edited != ModSync::Hash_Mod(alpha), "changing mod.ini changes the hash");

		Write_File(root / "Alpha" / "picture.png", "not hashed");
		std::uint32_t const with_picture = ModSync::Hash_Mod(alpha);
		Write_File(root / "Alpha" / "picture.png", "changed, still not hashed");
		Check(with_picture == ModSync::Hash_Mod(alpha), "a file mod.ini does not name is not hashed");

		ModClass missing = alpha;
		missing.RulesFile = (root / "Alpha" / "absent.ini").string();
		Check(ModSync::Hash_Mod(missing) != with_picture, "an overlay that is named but missing changes the hash");
	}

	/*
	 * Comparing finds what differs and says which mods are meant.
	 */
	{
		ModClass const alpha_fresh = Make_Mod(root, "Alpha", "[GAPOWR]\nCost=800\n");
		ModSync::Signature const both = Sign({alpha_fresh, beta});
		ModSync::Signature const only_alpha = Sign({alpha_fresh});
		ModSync::Signature const reversed = Sign({beta, alpha_fresh});
		ModSync::Signature const none = Sign({});

		ModSync::Difference host_has_more = Differ(only_alpha, both);
		Check(!host_has_more.Same() && host_has_more.OnlyThere.size() == 1 && host_has_more.OnlyThere[0] == "Beta" && host_has_more.OnlyHere.empty(), "a mod only the other machine has is named");

		ModSync::Difference host_has_less = Differ(both, only_alpha);
		Check(host_has_less.OnlyHere.size() == 1 && host_has_less.OnlyHere[0] == "Beta" && host_has_less.OnlyThere.empty(), "a mod only this machine has is named");

		ModSync::Difference order = Differ(both, reversed);
		Check(order.OrderDiffers && order.OnlyHere.empty() && order.OnlyThere.empty() && order.Changed.empty(), "the same mods in a different order differ by order");

		Write_File(root / "Beta" / "rules.ini", "[HTNK]\nCost=901\n");
		ModClass const beta_edited = Make_Mod(root, "Beta", "[HTNK]\nCost=901\n");
		ModSync::Difference changed = Differ(both, Sign({alpha_fresh, beta_edited}));
		Check(changed.Changed.size() == 1 && changed.Changed[0] == "Beta" && !changed.OrderDiffers, "a mod with different files is named as changed");

		Check(!Differ(none, both).Same() && !Differ(both, none).Same(), "a player with mods and a player without differ");

		ModSync::Signature upper = both;
		upper.Mods[0].Name = "ALPHA";
		Check(Differ(both, upper).Same(), "mod names compare without regard to case");

		std::string const text = ModSync::Describe_Difference(host_has_more, only_alpha, both, "The host");
		Check(text.find("Beta") != std::string::npos && text.find("The host uses mods: Alpha, Beta") != std::string::npos, "the message names the host's mods and the missing one");
		Check_Text(ModSync::Describe_Difference(order, both, reversed, "Bob"), "Bob uses mods: Beta, Alpha. You use: Alpha, Beta. The order differs.", "the message reports a different order");
	}

	/*
	 * A list too long for the packet is sent as a digest, which still tells equal lists from
	 * different ones.
	 */
	{
		std::vector<ModClass> many;
		for (int index = 0; index < 12; index++) {
			many.push_back(Make_Mod(root, "A mod with a rather long folder name " + std::to_string(index), "[X]\nY=" + std::to_string(index) + "\n"));
		}

		ModSync::Signature const full = Sign(many);
		std::string const text = ModSync::Encode(full);
		Check(text.size() < ModSync::CAPACITY && text[0] == '#', "a long list is written as a digest that fits");

		ModSync::Signature read;
		Check(ModSync::Decode(text, read) && read.IsDigest && read.DigestCount == 12, "a digest decodes with its count");
		Check(Differ(full, read).Same() && Differ(read, full).Same(), "a digest matches the full list it stands for");

		std::vector<ModClass> fewer(many.begin(), many.end() - 1);
		ModSync::Difference opaque = Differ(Sign(fewer), read);
		Check(opaque.IsOpaque && !opaque.Same(), "a digest differs from another list and cannot name the mods");
		Check_Text(ModSync::List_Names(read), "12 mods", "a digest is listed by its count");
	}

	/*
	 * Names that would break the text form are cleaned, and bad text is refused.
	 */
	{
		ModClass odd = alpha;
		odd.Folder = (root / "A,B=C").string() + std::string(1, static_cast<char>(std::filesystem::path::preferred_separator));
		ModSync::Signature const signature = Sign({odd});
		Check(signature.Mods[0].Name == "A_B_C", "separators in a folder name are replaced");

		ModSync::Signature read;
		Check(!ModSync::Decode("Alpha", read), "text with no hash is refused");
		Check(!ModSync::Decode("Alpha=12345", read), "a hash that is not eight digits is refused");
		Check(!ModSync::Decode("Alpha=1234567g", read), "a hash with a non-hex digit is refused");
		Check(!ModSync::Decode("=12345678", read), "an empty name is refused");
		Check(!ModSync::Decode("#x=12345678", read), "a digest with a bad count is refused");
		Check(read.Mods.empty(), "refused text reads as no mods");
		Check(ModSync::Decode("Alpha=0000abcd,Beta=FFFFFFFF", read) && read.Mods.size() == 2 && read.Mods[1].Hash == 0xFFFFFFFFu, "upper and lower case hex both read");
	}

	std::filesystem::remove_all(root);

	std::printf("%d failures\n", Failures);
	return(Failures == 0 ? 0 : 1);
}
