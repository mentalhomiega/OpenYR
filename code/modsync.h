/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "mods.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/*
 * A mod signature names the mod folders in force, in order, each with a hash of its mod.ini and
 * the overlay files that mod.ini names. A joining player sends his signature to the host, who
 * refuses a player whose signature differs from his own. The empty signature means no mods, and
 * is what a build that predates this check sends.
 *
 * The text form is "folder=hhhhhhhh,folder=hhhhhhhh", the hash in eight hexadecimal digits. A
 * list too long for the packet is sent as "#count=hhhhhhhh", one hash of the whole list, which
 * still shows that two lists differ but cannot say which mods differ.
 */
namespace ModSync
{
	// The room the text form has in a packet, terminating zero included.
	constexpr std::size_t CAPACITY = 384;

	struct Entry
	{
		std::string Name;
		std::uint32_t Hash = 0;
	};

	struct Signature
	{
		std::vector<Entry> Mods;

		// Set when the list was too long to send, so that Opaque_Hash stands in for the entries.
		bool IsDigest = false;
		std::uint32_t DigestCount = 0;
		std::uint32_t DigestHash = 0;
	};

	struct Difference
	{
		// Mods only this machine has, mods only the other machine has, and mods both have under
		// different contents.
		std::vector<std::string> OnlyHere;
		std::vector<std::string> OnlyThere;
		std::vector<std::string> Changed;

		// Both have the same mods but read them in a different order.
		bool OrderDiffers = false;

		// The lists differ but cannot be told apart by name.
		bool IsOpaque = false;

		bool Same(void) const;
	};

	std::uint32_t Hash_Bytes(std::uint32_t seed, void const * data, std::size_t length);
	std::uint32_t Hash_Mod(ModClass const & mod);

	Signature Describe(std::vector<ModClass> const & mods);
	std::string Encode(Signature const & signature);

	// Reads the text form. Text that is not a signature gives false, and counts as a signature
	// of no mods in the result.
	bool Decode(std::string_view text, Signature & signature);

	Difference Compare(Signature const & here, Signature const & there);

	// The names of the mods, in order, or "none".
	std::string List_Names(Signature const & signature);

	// Tells a player how the mods of who, such as "The host", differ from his own.
	std::string Describe_Difference(Difference const & difference, Signature const & here, Signature const & there, std::string const & who);
}
