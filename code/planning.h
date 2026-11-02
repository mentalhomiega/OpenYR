/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

class ObjectClass;

// The string table labels of the planning-mode messages that gamemd shows from PlanMgr.cpp.
inline constexpr char const * PLANNING_MSG_NODE_MAXIMUM = "MSG:NodeMaximum";
inline constexpr char const * PLANNING_MSG_PLANNER_MAXIMUM = "MSG:PlannerMaximum";
inline constexpr char const * PLANNING_MSG_POST_TERMINATING = "MSG:PostTerminatingCommand";
inline constexpr char const * PLANNING_MSG_POST_CONTINUAL = "MSG:PostContinualCommand";
inline constexpr char const * PLANNING_MSG_INTRO_KEY = "MSG:PlanningModeIntro1Key";
inline constexpr char const * PLANNING_MSG_INTRO_EXIT = "MSG:PlanningModeIntro3";
inline constexpr char const * PLANNING_MSG_NO_STOP = "MSG:PlanningModeNoStop";
inline constexpr char const * PLANNING_MSG_NO_SCATTER = "MSG:PlanningModeNoScatter";
inline constexpr char const * PLANNING_MSG_NO_DEPLOY = "MSG:PlanningModeNoDeploy";
inline constexpr char const * PLANNING_MSG_NO_GUARD_AREA = "MSG:PlanningModeNoGuardArea";
inline constexpr char const * PLANNING_MSG_NO_AIRCRAFT = "MSG:PlanningModeNoAircraft";
inline constexpr char const * PLANNING_MSG_NO_BUILDINGS = "MSG:PlanningModeNoBuildings";
inline constexpr char const * PLANNING_MSG_NO_UNKNOWNS = "MSG:PlanningModeNoUnknowns";

// Shows a planning-mode message for the local player, from its string table label, for gamemd's 0x1e0 ticks.
void Planning_Message(char const * label);

// Refuses a planning action: shows the message and plays the scold sound, at most once a frame, as gamemd's DAT_00ac4c08 guard.
void Planning_Refusal(char const * label);

// Sets the message that the end of the frame shows for the local player. A later call in the frame replaces an earlier one.
void Planning_Frame_Cue(char const * label);

// Shows the frame's pending message, if any, through Planning_Refusal. Called once the frame's events have run.
void Planning_Flush_Frame_Cue(void);

// The label of the message for a selected object that cannot take a plan, or NULL when it can: aircraft, buildings and other kinds.
char const * Planning_Unplannable_Label(ObjectClass const * object);

// The label of the refusal for the first selected object that cannot take a plan, or NULL when every selected object can.
char const * Planning_Selection_Label(void);

// Tells whether the selected units without plans would take the house past PLANNED_UNIT_LIMIT (gamemd's CanCreatePlanningNode, 0x639130).
bool Planning_Selection_Over_House_Limit(void);

// While one of these is alive with recording set, Queue_Mission turns the player's orders into planned orders.
class PlanningRecordScope
{
	public:
		explicit PlanningRecordScope(bool recording);
		~PlanningRecordScope(void);
		PlanningRecordScope(PlanningRecordScope const &) = delete;
		PlanningRecordScope & operator = (PlanningRecordScope const &) = delete;

	private:
		bool Previous;
};

// True while a PlanningRecordScope with recording set is alive.
bool Planning_Recording_Orders(void);
