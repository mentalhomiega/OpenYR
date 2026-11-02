/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "planning.h"

#include "_rules.h"
#include "aircraft.h"
#include "csf.h"
#include "dbgprint.h"
#include "dialog.hh"
#include "foot.h"
#include "ftimer.h"
#include "globals.h"
#include "house.h"
#include "playerdisplay.h"
#include "rules.h"
#include "session.h"
#include "voc.h"

#include <string>


namespace {
	// The frame of the last refusal cue. Refusals of every kind share it, as gamemd's DAT_00ac4c08 does.
	int Last_Planning_Refusal_Frame = -1;

	// The message the frame shows after its events, set by Planning_Frame_Cue.
	char const * Pending_Frame_Cue = NULL;

	bool Recording_Planned_Orders = false;
}


/// <summary>
/// Shows a planning-mode message to the local player from its string table label.
/// </summary>
/// <param name="label">The CSF label of the message, such as PLANNING_MSG_INTRO_KEY.</param>
/// <remarks>The message stays up for 0x1e0 ticks, the duration gamemd gives each planning message.</remarks>
void Planning_Message(char const * label)
{
	if (PlayerPtr == NULL) {
		return;
	}
	std::string const text = Fetch_String_UTF8(label);
	DebugString("Planning message %s: %s\n", label, text.c_str());
	Session.Messages.Add_Message(NULL, 0, text.c_str(), House_Display_Scheme(PlayerPtr),
		TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), 0x1E0);
}


/// <summary>
/// Refuses a planning action for the local player: the scold sound and the message.
/// </summary>
/// <param name="label">The CSF label of the message to show.</param>
/// <remarks>Only the first refusal of a frame is shown or heard. A later one in the same frame does nothing.</remarks>
void Planning_Refusal(char const * label)
{
	if (PlayerPtr == NULL || Last_Planning_Refusal_Frame == ::Frame) {
		return;
	}
	Last_Planning_Refusal_Frame = ::Frame;
	Sound_Effect(Rule->ScoldSound);
	Planning_Message(label);
}


/// <summary>
/// Gives the planning message for an object that cannot take a plan.
/// </summary>
/// <param name="object">The object to check.</param>
/// <returns>The label for aircraft, buildings and other kinds, or NULL when the object can take a plan.</returns>
char const * Planning_Unplannable_Label(ObjectClass const * object)
{
	if (object == NULL || !object->IsActive) {
		return(PLANNING_MSG_NO_UNKNOWNS);
	}
	if (object->RTTI == RTTI_AIRCRAFT) {
		if (Rule->AircraftWaypointReload && static_cast<AircraftClass const *>(object)->Reloads_At_Dock()) {
			return(NULL);
		}
		return(PLANNING_MSG_NO_AIRCRAFT);
	}
	if (object->Is_Foot()) {
		return(NULL);
	}
	if (object->RTTI == RTTI_BUILDING) {
		return(PLANNING_MSG_NO_BUILDINGS);
	}
	return(PLANNING_MSG_NO_UNKNOWNS);
}


/// <summary>
/// Gives the planning message for the first selected object that cannot take a plan.
/// </summary>
/// <returns>The label to show, or NULL when every selected object can take a plan.</returns>
char const * Planning_Selection_Label(void)
{
	for (int index = 0; index < CurrentObject.Count(); index++) {
		char const * label = Planning_Unplannable_Label(CurrentObject[index]);
		if (label != NULL) {
			return(label);
		}
	}
	return(NULL);
}


/// <summary>
/// Gives the message for a planned order that a unit cannot take.
/// </summary>
/// <param name="block">The reason from FootClass::Planned_Order_Block.</param>
/// <returns>The label, or NULL when the block is PLANNED_ORDER_OPEN.</returns>
char const * Planned_Order_Block_Label(PlannedOrderBlock block)
{
	switch (block) {
		case PLANNED_ORDER_AFTER_TERMINAL:
			return(PLANNING_MSG_POST_TERMINATING);

		case PLANNED_ORDER_AFTER_CONTINUAL:
			return(PLANNING_MSG_POST_CONTINUAL);

		case PLANNED_ORDER_NODE_FULL:
			return(PLANNING_MSG_NODE_MAXIMUM);

		case PLANNED_ORDER_HOUSE_FULL:
			return(PLANNING_MSG_PLANNER_MAXIMUM);

		case PLANNED_ORDER_OPEN:
		default:
			return(NULL);
	}
}


/// <summary>
/// Sets the message the frame shows for the local player when its events have run.
/// </summary>
/// <param name="label">The CSF label of the message.</param>
/// <remarks>gamemd keeps one pending message and overwrites it, so the last cue of a frame is the one shown
/// (PlanningNodeClass::ProcessPendingPlans, 0x637270).</remarks>
void Planning_Frame_Cue(char const * label)
{
	Pending_Frame_Cue = label;
}


/// <summary>
/// Shows the pending message of the frame, then clears it.
/// </summary>
void Planning_Flush_Frame_Cue(void)
{
	char const * label = Pending_Frame_Cue;
	Pending_Frame_Cue = NULL;
	if (label != NULL) {
		Planning_Refusal(label);
	}
}


/// <summary>
/// Tells whether the selection would take the house past its planned unit limit.
/// </summary>
/// <returns>bool; True when the house's planned units and the selected units without plans exceed PLANNED_UNIT_LIMIT.</returns>
/// <remarks>The planning click refuses the whole selection in this case, so no unit takes a plan from it.</remarks>
bool Planning_Selection_Over_House_Limit(void)
{
	if (PlayerPtr == NULL) {
		return(false);
	}
	int unplanned = 0;
	for (int index = 0; index < CurrentObject.Count(); index++) {
		ObjectClass const * object = CurrentObject[index];
		if (object->Is_Foot()) {
			FootClass const * foot = static_cast<FootClass const *>(object);
			if (foot->House == PlayerPtr && foot->PlannedOrders.Count() == 0) {
				unplanned++;
			}
		}
	}
	return(Planned_Unit_Count(PlayerPtr) + unplanned > PLANNED_UNIT_LIMIT);
}


PlanningRecordScope::PlanningRecordScope(bool recording)
	: Previous(Recording_Planned_Orders)
{
	Recording_Planned_Orders = recording;
}


PlanningRecordScope::~PlanningRecordScope(void)
{
	Recording_Planned_Orders = Previous;
}


bool Planning_Recording_Orders(void)
{
	return(Recording_Planned_Orders);
}
