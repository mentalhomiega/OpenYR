/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "coord.h"

/// Names derived from RadarEventNames. The values match gamemd's RadarEventType, which
/// indexes the per-kind table at 0x7F0998 (17 rows of 16 bytes).
enum RadarEventType {
	RADAREVENT_NONE=-1,

	RADAREVENT_COMBAT,				// 0
	RADAREVENT_NONCOMBAT,			// 1
	RADAREVENT_DROPZONE,			// 2
	RADAREVENT_BASE_ATTACKED,		// 3
	RADAREVENT_HARVESTER_ATTACKED,	// 4
	RADAREVENT_ENEMY_SENSED,		// 5
	RADAREVENT_FACTORY_OUTPUT,		// 6, call at 0x4FB631 in HouseClass::Place_Object
	RADAREVENT_UNIT_LOST,			// 7, call at 0x4D98FE in FootClass::Destroyed
	RADAREVENT_REPAIR,				// 8, call at 0x44B960 in BuildingClass::Mission_Repair
	RADAREVENT_INFILTRATED,			// 9, call at 0x45722F in BuildingClass::Infiltrated_By
	RADAREVENT_OWNER_CHANGED,		// 10, call at 0x448477 in BuildingClass::SetOwningHouse
	RADAREVENT_BEACON,				// 11, calls at 0x430F08 and 0x4316E5 in the beacon placement code
	RADAREVENT_BUILDING_PLACED,		// 12, call at 0x4468A8 in BuildingClass::Place
	RADAREVENT_SUPERWEAPON,			// 13, calls in SuperClass::Launch (0x6CC4BE) and LightningStorm::Start (0x539F89)
	RADAREVENT_FACTORY_FOUND,		// 14, call at 0x519BB6 in InfantryClass::FindFactory
	RADAREVENT_MIND_CONTROL,		// 15, call at 0x4582C5 in BuildingClass::Check_Mind_Control_Status
	RADAREVENT_ALLY_ATTACKED,		// 16, call at 0x4F95A0 in HouseClass::BuildingUnderAttack, ally branch

	RADAREVENT_COUNT,
	RADAREVENT_FIRST=0
};

/// Which colour pair a kind pulses in. Gamemd's Draw (0x660050) switches on the type: kinds 0, 3
/// and 4 are magenta, 1, 2, 11 and 12 yellow, 5 cyan, and every other kind has no colour. A kind
/// with no colour is never drawn, though it still suppresses and takes radar pixels.
enum RadarEventColorGroup {
	RADAREVENT_GROUP_MAGENTA,
	RADAREVENT_GROUP_YELLOW,
	RADAREVENT_GROUP_CYAN,
	RADAREVENT_GROUP_NONE
};

/// The per-kind data gamemd keeps as constants at 0x7F0998, stride 0x10 per kind.
struct RadarEventKindInfo {
	int SuppressionDistance;	// Cells; a same-kind event closer than this blocks the new one.
	int VisibilityFrames;		// Frames the settled box stays drawn.
	int DurationFrames;			// Frames the event stays in the list after it settles.
	bool Suppressible;			// Whether a same-kind event nearby can block a new one.
	RadarEventColorGroup ColorGroup;
};

inline constexpr RadarEventKindInfo RadarEventKinds[RADAREVENT_COUNT] = {
	{ 8, 200, 400, true,  RADAREVENT_GROUP_MAGENTA },	// COMBAT
	{ 8, 200, 400, false, RADAREVENT_GROUP_YELLOW },	// NONCOMBAT
	{ 8, 200, 400, false, RADAREVENT_GROUP_YELLOW },	// DROPZONE
	{ 8, 200, 600, true,  RADAREVENT_GROUP_MAGENTA },	// BASE_ATTACKED
	{ 8, 200, 400, true,  RADAREVENT_GROUP_MAGENTA },	// HARVESTER_ATTACKED
	{ 6, 200, 400, true,  RADAREVENT_GROUP_CYAN },		// ENEMY_SENSED
	{ 2, 0, 200, true,  RADAREVENT_GROUP_NONE },		// FACTORY_OUTPUT
	{ 8, 0, 200, true,  RADAREVENT_GROUP_NONE },		// UNIT_LOST
	{ 2, 0, 400, true,  RADAREVENT_GROUP_NONE },		// REPAIR
	{ 5, 0, 400, false, RADAREVENT_GROUP_NONE },		// INFILTRATED
	{ 8, 0, 100, false, RADAREVENT_GROUP_NONE },		// OWNER_CHANGED
	{ 8, 200, 200, true,  RADAREVENT_GROUP_YELLOW },	// BEACON
	{ 8, 200, 400, false, RADAREVENT_GROUP_YELLOW },	// BUILDING_PLACED
	{ 8, 0, 5, false, RADAREVENT_GROUP_NONE },			// SUPERWEAPON
	{ 8, 0, 200, true,  RADAREVENT_GROUP_NONE },		// FACTORY_FOUND
	{ 8, 0, 400, true,  RADAREVENT_GROUP_NONE },		// MIND_CONTROL
	{ 8, 200, 600, true,  RADAREVENT_GROUP_NONE },		// ALLY_ATTACKED
};

/// Milliseconds within which a second space-bar press steps back through the recent cells.
inline constexpr unsigned long RADAREVENT_JUMP_REPEAT_MS = 1600;

/// Is this a kind the table covers?
inline bool Radar_Event_Is_Valid(RadarEventType event)
{
	return event >= RADAREVENT_FIRST && event < RADAREVENT_COUNT;
}

/// Does an event already in play block a new one of this kind?
/// A blocking event is the same kind, its kind is suppressible, and the cells lie closer than the
/// kind's suppression distance. Gamemd compares the truncated distance with a strict less-than.
inline bool Radar_Event_Suppresses(RadarEventType proposed, Cell proposed_cell, RadarEventType existing, Cell existing_cell)
{
	if (!Radar_Event_Is_Valid(proposed) || !RadarEventKinds[proposed].Suppressible || existing != proposed) {
		return(false);
	}
	return(proposed_cell.Gamemd_Distance_To(existing_cell) < RadarEventKinds[proposed].SuppressionDistance);
}

/// Is a space-bar press close enough to the last one to step back through the recent cells?
inline bool Radar_Event_Jump_Repeats(unsigned long now, unsigned long last)
{
	return((now - last) < RADAREVENT_JUMP_REPEAT_MS);
}

/// The last eight event cells, for the space-bar jump. Gamemd's ring (0x660B80, 0x660BB0, 0x660BE0,
/// 0x660C50) keeps the newest at Head and a cursor that a repeat press walks backwards. A slot
/// still holding (0,0) is empty, so a walk that reaches one returns to the newest cell.
class RadarEventRecentCells
{
	public:
		RadarEventRecentCells(void) { Clear(); }

		void Push(Cell cell)
		{
			Cursor = (Head + 1) & 7;
			Head = Cursor;
			Cells[Head] = cell;
		}

		Cell Newest(void)
		{
			if (Head < 0) {
				return(Cell(0,0));
			}
			Cursor = Head;
			return(Cells[Head]);
		}

		Cell Step_Back(void)
		{
			if (Head < 0) {
				return(Cell(0,0));
			}
			int previous = Cursor - 1;
			if (previous < 0) {
				previous += 8;
			}
			if (Cells[previous] == Cell(0,0)) {
				Cursor = Head;
				return(Cells[Head]);
			}
			Cursor = previous;
			return(Cells[previous]);
		}

		void Clear(void)
		{
			for (int i = 0; i < 8; i++) {
				Cells[i] = Cell(0,0);
			}
			Head = -1;
			Cursor = -1;
		}

	private:
		Cell Cells[8];
		int Head;
		int Cursor;
};
