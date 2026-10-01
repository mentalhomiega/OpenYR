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

/****************************************************************************
**	The mouse cursor can be in different states. These states are listed
**	below. Some of these represent animating mouse cursors. The mouse
**	is controlled by passing one of these values to the appropriate
**	MouseClass member function.
*/
enum MouseType {
	MOUSE_NORMAL,
	MOUSE_N,
	MOUSE_NE,
	MOUSE_E,
	MOUSE_SE,
	MOUSE_S,
	MOUSE_SW,
	MOUSE_W,
	MOUSE_NW,
	MOUSE_NO_N,
	MOUSE_NO_NE,
	MOUSE_NO_E,
	MOUSE_NO_SE,
	MOUSE_NO_S,
	MOUSE_NO_SW,
	MOUSE_NO_W,
	MOUSE_NO_NW,
	MOUSE_CAN_SELECT,
	MOUSE_CAN_MOVE,
	MOUSE_NO_MOVE,
	MOUSE_STAY_ATTACK,
	MOUSE_CAN_ATTACK,
	MOUSE_AREA_GUARD,
	MOUSE_DESOLATOR_DEPLOY,
	MOUSE_CURSOR_18,
	MOUSE_ENTER,
	MOUSE_NO_ENTER,
	MOUSE_DEPLOY,
	MOUSE_NO_DEPLOY,
	MOUSE_UNDEPLOY,
	MOUSE_SELL_BACK,
	MOUSE_SELL_UNIT,
	MOUSE_NO_SELL_BACK,
	MOUSE_GREPAIR,
	MOUSE_REPAIR,
	MOUSE_NO_REPAIR,
	MOUSE_WAYPOINT,
	MOUSE_DISGUISE,
	MOUSE_IVAN_BOMB,
	MOUSE_MIND_CONTROL,
	MOUSE_REMOVE_SQUID,
	MOUSE_CRUSH,
	MOUSE_SPY_TECH,
	MOUSE_SPY_POWER,
	MOUSE_CURSOR_2C,
	MOUSE_GI_DEPLOY,
	MOUSE_CURSOR_2E,
	MOUSE_PARA_DROP,
	MOUSE_RALLY_POINT,
	MOUSE_CLOSE_WAYPOINT,
	MOUSE_LIGHTNING_STORM,
	MOUSE_DETONATE,
	MOUSE_DEMOLITIONS,
	MOUSE_NUCLEAR_BOMB,
	MOUSE_CURSOR_36,
	MOUSE_POWER,
	MOUSE_CURSOR_38,
	MOUSE_IRON_CURTAIN,
	MOUSE_CHRONOSPHERE,
	MOUSE_DISARM,
	MOUSE_DISALLOWED,
	MOUSE_SCROLL_COASTING,
	MOUSE_SCROLL_COASTING_N,
	MOUSE_SCROLL_COASTING_NE,
	MOUSE_SCROLL_COASTING_E,
	MOUSE_SCROLL_COASTING_SE,
	MOUSE_SCROLL_COASTING_S,
	MOUSE_SCROLL_COASTING_SW,
	MOUSE_SCROLL_COASTING_W,
	MOUSE_SCROLL_COASTING_NW,
	MOUSE_AREA_GUARD_2,
	MOUSE_CAN_ATTACK_2,
	MOUSE_LEAVE_BUILDING,
	MOUSE_INFANTRY_ABSORB,
	MOUSE_NO_MIND_CONTROL,
	MOUSE_NO_RALLY_POINT,
	MOUSE_CURSOR_4C,
	MOUSE_CURSOR_4D,
	MOUSE_BEACON,
	MOUSE_FORCE_SHIELD,
	MOUSE_NO_FORCE_SHIELD,
	MOUSE_GENETIC_MUTATOR,
	MOUSE_AIR_STRIKE,
	MOUSE_PSYCHIC_DOMINATOR,
	MOUSE_PSYCHIC_REVEAL,
	MOUSE_SPY_PLANE,

	MOUSE_COUNT,
	MOUSE_FIRST=0
};
