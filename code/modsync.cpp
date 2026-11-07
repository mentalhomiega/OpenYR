/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "modsync.h"
#include "diskfile.h"
#include "fnv.h"
#include "hexdigit.h"
#include "hexformat.h"
#include "strutil.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>

namespace ModSync
{
	namespace {

		constexpr std::size_t MAX_NAME_LENGTH = 40;


		// A name that cannot break the text form: no separators, no control characters, a limited length.
		std::string Clean_Name(std::string name)
		{
			for (char & c : name) {
				if (c == ',' || c == '=' || static_cast<unsigned char>(c) < 32) {
					c = '_';
				}
			}
			if (name.size() > MAX_NAME_LENGTH) {
				name.resize(MAX_NAME_LENGTH);
			}
			if (name.empty() || name[0] == '#') {
				name.insert(name.begin(), '_');
			}
			return(name);
		}


		// Adds the contents of a file to the hash, without its carriage returns, so that a file checked out with
		// either kind of line ending hashes the same. A file that cannot be read adds a different marker from an
		// empty one.
		std::uint32_t Hash_File(std::uint32_t hash, std::string const & path, unsigned char tag)
		{
			hash = Hash_Bytes(hash, &tag, 1);
			if (path.empty()) {
				unsigned char const none = 0;
				return(Hash_Bytes(hash, &none, 1));
			}

			std::string content;
			if (!Read_Disk_File(path, content)) {
				unsigned char const missing = 1;
				return(Hash_Bytes(hash, &missing, 1));
			}

			content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());

			unsigned char const present = 2;
			hash = Hash_Bytes(hash, &present, 1);
			std::uint32_t const length = static_cast<std::uint32_t>(content.size());
			hash = Hash_Bytes(hash, &length, sizeof(length));
			return(Hash_Bytes(hash, content.data(), content.size()));
		}


		std::string Entries_Text(Signature const & signature)
		{
			std::string text;
			for (Entry const & entry : signature.Mods) {
				if (!text.empty()) {
					text += ',';
				}
				text += entry.Name;
				text += "=" + Hex_Text(entry.Hash, 8, false);
			}
			return(text);
		}


		bool Parse_Hex(std::string_view text, std::uint32_t & value)
		{
			if (text.size() != 8) {
				return(false);
			}
			value = 0;
			for (char c : text) {
				int const digit = Hex_Digit_Value(c);
				if (digit < 0) {
					return(false);
				}
				value = (value << 4) | static_cast<std::uint32_t>(digit);
			}
			return(true);
		}


		// The count and hash that stand for the whole list when it is sent as a digest. text is
		// Entries_Text(signature) and is read only when the list is not already a digest.
		void Digest_Of(Signature const & signature, std::string const & text, std::uint32_t & count, std::uint32_t & hash)
		{
			if (signature.IsDigest) {
				count = signature.DigestCount;
				hash = signature.DigestHash;
				return;
			}
			count = static_cast<std::uint32_t>(signature.Mods.size());
			hash = Hash_Bytes(FNV_BASIS, text.data(), text.size());
		}


		// Returns the first entry named lower_name ignoring case, or nullptr; lower_name must already be lower case.
		Entry const * Find_Mod(Signature const & signature, std::string const & lower_name)
		{
			auto found = std::find_if(signature.Mods.begin(), signature.Mods.end(), [&](Entry const & entry) {return(Lower(entry.Name) == lower_name);});
			return(found == signature.Mods.end() ? nullptr : &*found);
		}


		void Append_Names(std::string & text, std::vector<std::string> const & names)
		{
			for (std::size_t index = 0; index < names.size(); index++) {
				text += index == 0 ? "" : ", ";
				text += names[index];
			}
		}

	} // namespace


	bool Difference::Same(void) const
	{
		return(OnlyHere.empty() && OnlyThere.empty() && Changed.empty() && !OrderDiffers && !IsOpaque);
	}


	std::uint32_t Hash_Bytes(std::uint32_t seed, void const * data, std::size_t length)
	{
		return(Fnv_Add_Bytes(seed, data, length));
	}


	/// <summary>
	/// Hashes the files that make a mod what it is: its mod.ini, then the rules, art and AI
	/// overlays mod.ini names. Carriage returns are left out, so that the same file with either
	/// kind of line ending gives the same hash.
	/// </summary>
	std::uint32_t Hash_Mod(ModClass const & mod)
	{
		std::uint32_t hash = FNV_BASIS;
		hash = Hash_File(hash, mod.Folder + "mod.ini", 'M');
		hash = Hash_File(hash, mod.RulesFile, 'R');
		hash = Hash_File(hash, mod.ArtFile, 'A');
		hash = Hash_File(hash, mod.AIFile, 'I');
		return(hash);
	}


	Signature Describe(std::vector<ModClass> const & mods)
	{
		Signature signature;
		for (ModClass const & mod : mods) {
			Entry entry;
			entry.Name = Clean_Name(Mod_Folder_Own_Name(mod.Folder));
			entry.Hash = Hash_Mod(mod);
			signature.Mods.push_back(entry);
		}
		return(signature);
	}


	/// <summary>
	/// Writes the signature as text that fits CAPACITY. A list too long for that becomes a digest.
	/// </summary>
	std::string Encode(Signature const & signature)
	{
		std::uint32_t count;
		std::uint32_t hash;
		std::string const text = Entries_Text(signature);

		if (!signature.IsDigest && text.size() < CAPACITY) {
			return(text);
		}

		Digest_Of(signature, text, count, hash);
		return("#" + std::to_string(count) + "=" + Hex_Text(hash, 8, false));
	}


	bool Decode(std::string_view text, Signature & signature)
	{
		signature = Signature();
		if (text.empty()) {
			return(true);
		}

		if (text[0] == '#') {
			std::string_view::size_type const equals = text.find('=');
			if (equals == std::string_view::npos || equals < 2 || equals > 11) {
				return(false);
			}
			std::uint32_t count = 0;
			for (char c : text.substr(1, equals - 1)) {
				if (c < '0' || c > '9') {
					return(false);
				}
				count = count * 10 + static_cast<std::uint32_t>(c - '0');
			}
			std::uint32_t hash;
			if (!Parse_Hex(text.substr(equals + 1), hash)) {
				return(false);
			}
			signature.IsDigest = true;
			signature.DigestCount = count;
			signature.DigestHash = hash;
			return(true);
		}

		Signature parsed;
		while (!text.empty()) {
			std::string_view::size_type const comma = text.find(',');
			std::string_view const token = text.substr(0, comma);
			text = comma == std::string_view::npos ? std::string_view() : text.substr(comma + 1);

			std::string_view::size_type const equals = token.rfind('=');
			Entry entry;
			if (equals == std::string_view::npos || equals == 0 || !Parse_Hex(token.substr(equals + 1), entry.Hash)) {
				return(false);
			}
			entry.Name = std::string(token.substr(0, equals));
			parsed.Mods.push_back(entry);
		}

		signature = parsed;
		return(true);
	}


	Difference Compare(Signature const & here, Signature const & there)
	{
		Difference difference;

		if (here.IsDigest || there.IsDigest) {
			std::uint32_t count_here, hash_here, count_there, hash_there;
			Digest_Of(here, Entries_Text(here), count_here, hash_here);
			Digest_Of(there, Entries_Text(there), count_there, hash_there);
			difference.IsOpaque = count_here != count_there || hash_here != hash_there;
			return(difference);
		}

		for (Entry const & mine : here.Mods) {
			Entry const * theirs = Find_Mod(there, Lower(mine.Name));
			if (theirs == nullptr) {
				difference.OnlyHere.push_back(mine.Name);
			} else if (theirs->Hash != mine.Hash) {
				difference.Changed.push_back(mine.Name);
			}
		}

		for (Entry const & theirs : there.Mods) {
			if (Find_Mod(here, Lower(theirs.Name)) == nullptr) {
				difference.OnlyThere.push_back(theirs.Name);
			}
		}

		if (difference.OnlyHere.empty() && difference.OnlyThere.empty() && difference.Changed.empty()
			&& here.Mods.size() == there.Mods.size()) {
			for (std::size_t index = 0; index < here.Mods.size(); index++) {
				if (Lower(here.Mods[index].Name) != Lower(there.Mods[index].Name)) {
					difference.OrderDiffers = true;
					break;
				}
			}
		}

		return(difference);
	}


	std::string List_Names(Signature const & signature)
	{
		if (signature.IsDigest) {
			return(std::to_string(signature.DigestCount) + (signature.DigestCount == 1 ? " mod" : " mods"));
		}
		if (signature.Mods.empty()) {
			return("none");
		}

		std::vector<std::string> names;
		for (Entry const & entry : signature.Mods) {
			names.push_back(entry.Name);
		}
		std::string text;
		Append_Names(text, names);
		return(text);
	}


	std::string Describe_Difference(Difference const & difference, Signature const & here, Signature const & there, std::string const & who)
	{
		std::string text = who + " uses mods: " + List_Names(there) + ". You use: " + List_Names(here) + ".";

		// A digest carries no names, so the message cannot say which mods differ.
		if (difference.IsOpaque) {
			text += " The mod lists differ in names, order, or files.";
		}
		if (!difference.OnlyThere.empty()) {
			text += " You lack: ";
			Append_Names(text, difference.OnlyThere);
			text += ".";
		}
		if (!difference.OnlyHere.empty()) {
			text += " " + who + " lacks: ";
			Append_Names(text, difference.OnlyHere);
			text += ".";
		}
		if (!difference.Changed.empty()) {
			text += " Files differ in: ";
			Append_Names(text, difference.Changed);
			text += ".";
		}
		if (difference.OrderDiffers) {
			text += " The order differs.";
		}
		return(text);
	}
}
