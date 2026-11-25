/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "peercheck.h"

#include <cctype>

namespace PeerCheck
{
	// Names the files whose identities differ, such as "rules, art".
	static std::string Differing_Data(Facts const & here, Facts const & there)
	{
		std::string text;
		auto add = [&text](char const * name) {
			text += text.empty() ? "" : ", ";
			text += name;
		};
		if (here.Rules != there.Rules) {
			add("rules");
		}
		if (here.Art != there.Art) {
			add("art");
		}
		if (here.AI != there.AI) {
			add("AI");
		}
		return(text);
	}


	Verdict Judge(Facts const & here, Facts const & there, std::string const & who)
	{
		Verdict verdict;

		if (here.Build != there.Build) {
			verdict.Differs = Level::BUILD;
			verdict.Text = who + " runs game build " + std::to_string(there.Build) + ". You run build " + std::to_string(here.Build) + ".";
			return(verdict);
		}

		ModSync::Difference const difference = ModSync::Compare(here.Mods, there.Mods);
		if (!difference.Same()) {
			verdict.Differs = Level::MODS;
			verdict.Text = ModSync::Describe_Difference(difference, here.Mods, there.Mods, who);
			return(verdict);
		}

		std::string const data = Differing_Data(here, there);
		if (!data.empty()) {
			verdict.Differs = Level::DATA;
			verdict.Text = who + " has different game data. These files differ: " + data + ".";
		}
		return(verdict);
	}


	Mode Mode_From_Text(std::string const & text)
	{
		std::string lower;
		for (char ch : text) {
			lower += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
		}
		if (lower == "warn") {
			return(Mode::WARN);
		}
		if (lower == "off") {
			return(Mode::OFF);
		}
		return(Mode::REFUSE);
	}


	void Roster::Clear(void)
	{
		for (std::optional<Facts> & report : Reports) {
			report.reset();
		}
	}


	void Roster::Note(std::size_t seat, Facts const & facts)
	{
		if (seat < SEATS) {
			Reports[seat] = facts;
		}
	}


	bool Roster::Has(std::size_t seat) const
	{
		return(seat < SEATS && Reports[seat].has_value());
	}


	Facts const & Roster::Get(std::size_t seat) const
	{
		static Facts const none;
		return(Has(seat) ? *Reports[seat] : none);
	}
}
