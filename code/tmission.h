/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#pragma once

#include "dialog.hh"
#include "facing.hh"
#include "house.hh"
#include "mission.hh"
#include "quarry.hh"
#include "scrspeed.hh"
#include "target.hh"
#include "theme.hh"
#include "tmission.hh"
#include "voc.hh"
#include "vox.hh"
#include "vq.hh"

/// Whether Move to Cell lines on a map with NewINIFormat 4 or higher carry their cell as 1000 * Y + X instead of
/// gamemd's 128 * Y + X. [General] MoveToCellWide= decides when set; otherwise rules that declare Ares armor
/// types read the wide form and others keep gamemd's, whatever the stock maps' own NewINIFormat of 4 says.
inline bool Move_To_Cell_Is_Wide(bool key_set, bool key_value, int declared_armor_types)
{
	return(key_set ? key_value : declared_armor_types > 0);
}

/*
**	This structure contains one team mission value & its argument.
*/
class TeamMissionClass
{
	public:
		TeamMissionClass(void) {}
		TeamMissionClass(TeamMissionType mission, int data) : Mission(mission) { Data.Value = data; }

		void Fill_In(char const * entry);
		int Build_INI_Entry(char * ptr) const;

		/*
		 * Carries the mission entry to or from a save game. Which alternative of the data
		 * is live depends on the mission, but every one of them is a plain scalar and none
		 * holds a pointer, so the whole union travels as its raw image.
		 */
		template<typename S>
		void Serialize(S & stream)
		{
			stream.Serialize(Mission);
			stream.Serialize_Bytes(&Data, sizeof(Data));
		}

		TeamMissionType Mission;		// Mission type.
		union {
			QuarryType		Quarry;			// Combat quarry type.
			MissionType		Mission;		// General mission orders.
			VQType			Movie;
			VocType			Sound;
			VoxType			Speech;
			ScrollSpeedType	Speed;
			ThemeType 		Theme;
			HousesType		House;
			FacingType		Facing;
			int Value;						// Usually a waypoint number.
			struct {
				unsigned int Type : 16;
				TargetPropertyType Prop : 16;
			};
			struct {
				unsigned int AType : 16;
				signed int ALoops : 16;
			};
		} Data;
};

extern char const * const TMissions[TMISSION_COUNT];
extern char const * const TMissionsHelp[TMISSION_COUNT];
