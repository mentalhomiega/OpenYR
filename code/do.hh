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
**	Infantry can be performing various activities. These can range from simple
**	idle animations to physical hand to hand combat.
*/
enum DoType {
	DO_NOTHING=-1,				// Not performing any choreographed sequence.

	DO_STAND_READY=0,
	DO_STAND_GUARD,
	DO_PRONE,
	DO_WALK,
	DO_FIRE_WEAPON,
	DO_LIE_DOWN,
	DO_CRAWL,
	DO_GET_UP,
	DO_FIRE_PRONE,
	DO_IDLE1,
	DO_IDLE2,
	DO_GUN_DEATH,
	DO_EXPLOSION_DEATH,
	DO_EXPLOSION2_DEATH,
	DO_GRENADE_DEATH,
	DO_FIRE_DEATH,
	DO_HOVER,
	DO_FLY,
	DO_TUMBLE,
	DO_FIREFLY,
	DO_STRUGGLE,

	// Yuri's Revenge sequences, in the order of its art sequence names.
	DO_TREAD,
	DO_SWIM,
	DO_WET_IDLE1,
	DO_WET_IDLE2,
	DO_WET_DIE1,
	DO_WET_DIE2,
	DO_WET_ATTACK,
	DO_DEPLOY,
	DO_DEPLOYED,
	DO_DEPLOYED_FIRE,
	DO_DEPLOYED_IDLE,
	DO_UNDEPLOY,
	DO_CHEER,
	DO_PARADROP,
	DO_AIR_DEATH_START,
	DO_AIR_DEATH_FALLING,
	DO_AIR_DEATH_FINISH,
	DO_PANIC,
	DO_SHOVEL,
	DO_CARRY,
	DO_SECONDARY_FIRE,
	DO_SECONDARY_PRONE,

	DO_COUNT,
	DO_FIRST=0
};
