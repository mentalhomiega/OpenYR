/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cctype>
#include <cstddef>
#include <string>


/*
 * A battle file's Scenario= value may name the archive its map is read from, as RA2->ALL01T.MAP
 * names ALL01T.MAP in Red Alert 2's archives. Only RA2 is recognized; any other text is the file
 * name as written. Returns true when an archive prefix was split off.
 */
inline bool Campaign_Split_Scenario(std::string const & value, std::string & archive, std::string & file)
{
	archive.clear();
	file = value;

	std::size_t const arrow = value.find("->");
	if (arrow == std::string::npos || arrow == 0) {
		return(false);
	}

	std::string prefix = value.substr(0, arrow);
	for (char & ch : prefix) {
		ch = (char)std::toupper((unsigned char)ch);
	}
	if (prefix != "RA2") {
		return(false);
	}

	archive = "RA2";
	file = value.substr(arrow + 2);
	return(true);
}


/*
 * Which side a campaign's first map belongs to. The battle files name Allied maps with an ALL
 * prefix and Soviet maps with an SOV prefix; anything else, such as a tutorial, is neither. The
 * map name is upper case.
 */
enum CampaignSideType {
	CAMPAIGN_SIDE_ALLIED,
	CAMPAIGN_SIDE_SOVIET,
	CAMPAIGN_SIDE_OTHER
};

inline CampaignSideType Campaign_Side_Of_Map(std::string const & map)
{
	if (map.compare(0, 3, "ALL") == 0) {
		return(CAMPAIGN_SIDE_ALLIED);
	}
	if (map.compare(0, 3, "SOV") == 0) {
		return(CAMPAIGN_SIDE_SOVIET);
	}
	return(CAMPAIGN_SIDE_OTHER);
}


/*
 * Is this a map file name? A campaign whose Scenario= is not a map, such as a movie sequence or a
 * header line, cannot start a mission.
 */
inline bool Campaign_Is_Mission_File(std::string const & file)
{
	if (file.size() < 5 || file.find("->") != std::string::npos) {
		return(false);
	}

	std::string extension = file.substr(file.size() - 4);
	for (char & ch : extension) {
		ch = (char)std::toupper((unsigned char)ch);
	}
	return(extension == ".MAP");
}
