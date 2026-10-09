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

/* $Header: /CounterStrike/COMBAT.CPP 1     3/03/97 10:24a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : COMBAT.CPP                                                   *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : September 19, 1994                                           *
 *                                                                                             *
 *                  Last Update : July 26, 1996 [JLB]                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   Combat_Anim -- Determines explosion animation to play.                                    *
 *   Explosion_Damage -- Inflict an explosion damage affect.                                   *
 *   Modify_Damage -- Adjusts damage to reflect the nature of the target.                      *
 *   Wide_Area_Damage -- Apply wide area damage to the map.                                    *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "combat.h"

#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "globals.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "isotype.h"
#include "mouse.h"
#include "overtype.h"
#include "ovrlight.h"
#include "partsys.h"
#include "psystype.h"
#include "rules.h"
#include "scenario.h"
#include "stimer.h"
#include "tactical.h"
#include "team.h"
#include "teamtype.h"
#include "tiberium.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vein.h"
#include "warhead.h"

#include "mph.hh"

#include <algorithm>
#include <vector>
#include <windef.h>


/***********************************************************************************************
 * Modify_Damage -- Adjusts damage to reflect the nature of the target.                        *
 *                                                                                             *
 *    This routine is the core of combat tactics. It implements the                            *
 *    affect various armor types have against various weapon types. By                         *
 *    careful exploitation of this table, tactical advantage can be                            *
 *    obtained.                                                                                *
 *                                                                                             *
 * INPUT:   damage   -- The damage points to process.                                          *
 *                                                                                             *
 *          warhead  -- The source of the damage points.                                       *
 *                                                                                             *
 *          armor    -- The type of armor defending against the damage.                        *
 *                                                                                             *
 *          distance -- The distance (in leptons) from the source of the damage.               *
 *                                                                                             *
 * OUTPUT:  Returns with the adjusted damage points to inflict upon the                        *
 *          target.                                                                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/16/1994 JLB : Created.                                                                 *
 *   04/17/1994 JLB : Always does a minimum of damage.                                         *
 *   01/01/1995 JLB : Takes into account distance from damage source.                          *
 *   04/11/1996 JLB : Changed damage fall-off formula for less damage fall-off.                *
 *=============================================================================================*/
int Modify_Damage(int damage, WarheadTypeClass const * warhead, ArmorType armor, int distance)
{
	/*
	**	If there is no raw damage value to start with, then
	**	there can be no modified damage either.
	*/
	if (!damage || Scen->Special.IsInert || warhead == NULL) return(0);

	/*
	**	Negative damage (i.e., heal) is always applied full strength, but only if the heal
	**	effect is close enough.
	*/
	if (damage < 0) {
		if (distance < 0x008) return(damage);
		return(0);
	}

	/*
	**	Damage falls off in a straight line from the full amount at the point of impact to
	**	PercentAtMax of it at CellSpread cells, and only then meets the warhead's verses
	**	against the armor (MapClass::GetTotalDamage, 0x489180).
	*/
	float const full = (float)damage;
	float const edge = full * warhead->PercentAtMax;
	int const spread = (int)(warhead->CellSpread * 256.0f);
	if (edge != full && spread != 0) {
		// Double precision matches gamemd's x87 steps; in float a distance-0 hit can lose a point.
		damage = (int)(((double)full - (double)edge) * (spread - distance) / spread + (double)edge);
	}
	damage = std::max(damage, 0);
	damage = (int)(damage * warhead->Versus(armor));

	damage = std::min(damage, Rule->MaxDamage);
	return(damage);
}


/// <summary>
/// Handles the chain reaction of a tiberium field.
/// This routine is called when a cell of chain reactive growth is disturbed. The tiberium
/// there goes up, damaging whatever is standing in the blast, and the neighboring growth
/// is set off in turn so that the reaction ripples across the field.
/// </summary>
/// <param name="cell">The cell whose growth should be set off.</param>
void Chain_Reaction_Damage(Cell const & cell)
{
	CellClass * cellptr = &Map[cell];
	TiberiumType tiberium = cellptr->Tiberium_Type_Here();

	TiberiumClass * tptr;
	if (tiberium != TIBERIUM_NONE) {
		tptr = Tiberiums[tiberium];
	} else {
		tptr = NULL;
	}

	OverlayTypeClass * optr;
	if (cellptr->Overlay != OVERLAY_NONE) {
		optr = OverlayTypes[cellptr->Overlay];
	} else {
		optr = NULL;
	}

	if (tptr != NULL && optr != NULL && optr->IsChainReaction && cellptr->OverlayData > 1) {

		Coord coord = cellptr->Cell_Coord();
		if (Percent_Chance(5 * cellptr->OverlayData > 0 ? 5 * cellptr->OverlayData : 0)) {
			int amount = cellptr->OverlayData / 2;
			int damage = amount * tptr->Power;
			bool grow = false;
			if (cellptr->OverlayData >= OVERLAYDATA_TIBERIUM_MAX) {
				grow = true;
			}
			cellptr->OverlayData -= amount;
			cellptr->Register_For_Redraw();

			AnimTypeClass const * anim = Combat_Anim(4 * damage, Rule->C4Warhead, cellptr->Land_Type(), coord);
			if (anim != NULL) {
				new AnimClass(anim, coord);
			}
			Explosion_Damage(coord, damage, 0, Rule->C4Warhead, false);

			for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++){

				Coord adjacent = Adjacent_Coord_With_Height(coord, dir);
				adjacent.Z = Map[adjacent].Get_Height();

				CellClass * adjacent_ptr = &Map[adjacent];
				if (adjacent_ptr->Tiberium_Type_Here() != TIBERIUM_NONE && adjacent_ptr->OverlayData > 2 && Percent_Chance(80)) {
					new AnimClass(AnimTypes[AnimTypeClass::From_Name("INVISO")], adjacent, Random_Pick(TICKS_PER_SECOND, 8 * TICKS_PER_SECOND));
				}
			}

			if (grow) {
				tptr->Queue_Growth(cell);
			}
		}
	}
}


/// <summary>
/// Determines the distance between two points, ignoring a slight height difference.
/// This routine is used when explosion damage is being dealt out, so that a victim
/// perched a fraction of a level above or below the blast is not judged to be further
/// away than one standing level with it.
/// </summary>
/// <returns>Returns with the distance between the two coordinates.</returns>
inline static int Explosion_Distance(Coord const & coord1, Coord const & coord2)
{
	int z1 = coord1.Z;
	int z2 = coord2.Z;
	if (abs(z2 - z1) < LEVEL_LEPTON_H) {
		z2 = coord1.Z;
	}
	return(Coord(coord1.X - coord2.X, coord1.Y - coord2.Y, z1 - z2).Length());
}


/*
 * The cells a blast of each radius reaches, nearest first, in the order of gamemd's CellSpreadTable
 * (0xABD490). Ring r is the entries from Cell_Spread_Count(r - 1) up to Cell_Spread_Count(r). Rings 0 to
 * 10 follow the distance rule, the longer offset plus half the shorter one. Ring 11 does not: it swaps
 * (+-2, +-10) and (+-3, +-10) for (+-2, +-11), (-3, -11), (-3, 11) and (3, 11), and (-3, 11) is listed twice.
 */
int Cell_Spread_Count(int radius)
{
	static int const _counts[CELL_SPREAD_MAX + 1] = {1, 9, 21, 37, 61, 89, 121, 161, 205, 253, 309, 369};
	return(_counts[std::clamp(radius, 0, CELL_SPREAD_MAX)]);
}

Cell Cell_Spread_Offset(int index)
{
	static signed char const _offsets[][2] = {
		{0, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1},
		{-1, -2}, {0, -2}, {1, -2}, {-2, -1}, {2, -1}, {-2, 0}, {2, 0}, {-2, 1}, {2, 1},
		{-1, 2}, {0, 2}, {1, 2}, {-1, -3}, {0, -3}, {1, -3}, {-2, -2}, {2, -2}, {-3, -1},
		{3, -1}, {-3, 0}, {3, 0}, {-3, 1}, {3, 1}, {-2, 2}, {2, 2}, {-1, 3}, {0, 3},
		{1, 3}, {-1, -4}, {0, -4}, {1, -4}, {-3, -3}, {-2, -3}, {2, -3}, {3, -3}, {-3, -2},
		{3, -2}, {-4, -1}, {4, -1}, {-4, 0}, {4, 0}, {-4, 1}, {4, 1}, {-3, 2}, {3, 2},
		{-3, 3}, {-2, 3}, {2, 3}, {3, 3}, {-1, 4}, {0, 4}, {1, 4}, {-1, -5}, {0, -5},
		{1, -5}, {-3, -4}, {-2, -4}, {2, -4}, {3, -4}, {-4, -3}, {4, -3}, {-4, -2}, {4, -2},
		{-5, -1}, {5, -1}, {-5, 0}, {5, 0}, {-5, 1}, {5, 1}, {-4, 2}, {4, 2}, {-4, 3},
		{4, 3}, {-3, 4}, {-2, 4}, {2, 4}, {3, 4}, {-1, 5}, {0, 5}, {1, 5}, {-1, -6},
		{0, -6}, {1, -6}, {-3, -5}, {-2, -5}, {2, -5}, {3, -5}, {-4, -4}, {4, -4}, {-5, -3},
		{5, -3}, {-5, -2}, {5, -2}, {-6, -1}, {6, -1}, {-6, 0}, {6, 0}, {-6, 1}, {6, 1},
		{-5, 2}, {5, 2}, {-5, 3}, {5, 3}, {-4, 4}, {4, 4}, {-3, 5}, {-2, 5}, {2, 5},
		{3, 5}, {-1, 6}, {0, 6}, {1, 6}, {-1, -7}, {0, -7}, {1, -7}, {-3, -6}, {-2, -6},
		{2, -6}, {3, -6}, {-5, -5}, {-4, -5}, {4, -5}, {5, -5}, {-5, -4}, {5, -4}, {-6, -3},
		{6, -3}, {-6, -2}, {6, -2}, {-7, -1}, {7, -1}, {-7, 0}, {7, 0}, {-7, 1}, {7, 1},
		{-6, 2}, {6, 2}, {-6, 3}, {6, 3}, {-5, 4}, {5, 4}, {-5, 5}, {-4, 5}, {4, 5},
		{5, 5}, {-3, 6}, {-2, 6}, {2, 6}, {3, 6}, {-1, 7}, {0, 7}, {1, 7}, {-1, -8},
		{0, -8}, {1, -8}, {-3, -7}, {-2, -7}, {2, -7}, {3, -7}, {-5, -6}, {-4, -6}, {4, -6},
		{5, -6}, {-6, -5}, {6, -5}, {-6, -4}, {6, -4}, {-7, -3}, {7, -3}, {-7, -2}, {7, -2},
		{-8, -1}, {8, -1}, {-8, 0}, {8, 0}, {-8, 1}, {8, 1}, {-7, 2}, {7, 2}, {-7, 3},
		{7, 3}, {-6, 4}, {6, 4}, {-6, 5}, {6, 5}, {-5, 6}, {-4, 6}, {4, 6}, {5, 6},
		{-3, 7}, {-2, 7}, {2, 7}, {3, 7}, {-1, 8}, {0, 8}, {1, 8}, {-1, -9}, {0, -9},
		{1, -9}, {-3, -8}, {-2, -8}, {2, -8}, {3, -8}, {-5, -7}, {-4, -7}, {4, -7}, {5, -7},
		{-6, -6}, {6, -6}, {-7, -5}, {7, -5}, {-7, -4}, {7, -4}, {-8, -3}, {8, -3}, {-8, -2},
		{8, -2}, {-9, -1}, {9, -1}, {-9, 0}, {9, 0}, {-9, 1}, {9, 1}, {-8, 2}, {8, 2},
		{-8, 3}, {8, 3}, {-7, 4}, {7, 4}, {-7, 5}, {7, 5}, {-6, 6}, {6, 6}, {-5, 7},
		{-4, 7}, {4, 7}, {5, 7}, {-3, 8}, {-2, 8}, {2, 8}, {3, 8}, {-1, 9}, {0, 9},
		{1, 9}, {-1, -10}, {0, -10}, {1, -10}, {-3, -9}, {-2, -9}, {2, -9}, {3, -9}, {-5, -8},
		{-4, -8}, {4, -8}, {5, -8}, {-7, -7}, {-6, -7}, {6, -7}, {7, -7}, {-7, -6}, {7, -6},
		{-8, -5}, {8, -5}, {-8, -4}, {8, -4}, {-9, -3}, {9, -3}, {-9, -2}, {9, -2}, {-10, -1},
		{10, -1}, {-10, 0}, {10, 0}, {-10, 1}, {10, 1}, {-9, 2}, {9, 2}, {-9, 3}, {9, 3},
		{-8, 4}, {8, 4}, {-8, 5}, {8, 5}, {-7, 6}, {7, 6}, {-7, 7}, {-6, 7}, {6, 7},
		{7, 7}, {-5, 8}, {-4, 8}, {4, 8}, {5, 8}, {-3, 9}, {-2, 9}, {2, 9}, {3, 9},
		{-1, 10}, {0, 10}, {1, 10}, {0, 11}, {0, -11}, {-1, 11}, {1, 11}, {-1, -11}, {1, -11},
		{-2, 11}, {2, 11}, {-2, -11}, {2, -11}, {-3, 11}, {3, 11}, {-3, -11}, {-3, 11}, {-4, 9},
		{4, 9}, {-4, -9}, {4, -9}, {-5, 9}, {5, 9}, {-5, -9}, {5, -9}, {-6, 8}, {6, 8},
		{-6, -8}, {6, -8}, {-7, 8}, {7, 8}, {-7, -8}, {7, -8}, {-8, 7}, {8, 7}, {-8, -7},
		{8, -7}, {-8, 6}, {8, 6}, {-8, -6}, {8, -6}, {-9, 5}, {9, 5}, {-9, -5}, {9, -5},
		{-9, 4}, {9, 4}, {-9, -4}, {9, -4}, {-10, 3}, {10, 3}, {-10, -3}, {10, -3}, {-10, 2},
		{10, 2}, {-10, -2}, {10, -2}, {-11, 1}, {11, 1}, {-11, -1}, {11, -1}, {11, 0}, {-11, 0}
	};
	return(Cell(_offsets[index][0], _offsets[index][1]));
}


/***********************************************************************************************
 * Explosion_Damage -- Inflict an explosion damage affect.                                     *
 *                                                                                             *
 *    Processes the collateral damage affects typically caused by an                           *
 *    explosion.                                                                               *
 *                                                                                             *
 * INPUT:   coord    -- The coordinate of ground zero.                                         *
 *                                                                                             *
 *          strength -- Raw damage points at ground zero.                                      *
 *                                                                                             *
 *          source   -- Source of the explosion (who is responsible).                          *
 *                                                                                             *
 *          warhead  -- The kind of explosion to process.                                      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   This routine can consume some time and will affect the AI                       *
 *             of nearby enemy units (possibly).                                               *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/16/1991 JLB : Created.                                                                 *
 *   11/30/1991 JLB : Uses coordinate system.                                                  *
 *   12/27/1991 JLB : Radius of explosion damage effect.                                       *
 *   04/13/1994 JLB : Streamlined.                                                             *
 *   04/16/1994 JLB : Warhead damage type modifier.                                            *
 *   04/17/1994 JLB : Cleaned up.                                                              *
 *   06/20/1994 JLB : Uses object pointers to distribute damage.                               *
 *   06/20/1994 JLB : Source is a pointer.                                                     *
 *   06/18/1996 JLB : Strength could be negative for healing effects.                          *
 *=============================================================================================*/
HouseClass * DamageSourceHouse = NULL;


bool Explosion_Damage(Coord const & coord, int strength, TechnoClass * source, WarheadTypeClass const * warhead, bool dochainreaction, HouseClass * sourcehouse)
{
	Cell								cell;		// Cell number under explosion.
	ObjectClass *						object;		// Working object pointer.
	DynamicVectorClass<ObjectClass *>	objects;	// Maximum number of objects that can be damaged.
	int									distance;	// Distance to unit.
	int									range;		// Damage effect radius.

	if (Scen->Special.IsInert || warhead == NULL) return(false);

	if (!strength && !warhead->IsWebby) return(false);

	/*
	 * The blast reaches CellSpread cells. Every object in that reach is listed with its distance
	 * from the blast, and damaged once the list is complete (MapClass::DamageArea, 0x489280).
	 */
	range = (int)(warhead->CellSpread * CELL_LEPTON_W);
	bool const pinpoint = warhead->CellSpread <= 0.5f;
	bool curtain_hit = false;
	cell = coord.As_Cell();

	CellClass * cellptr = &Map[cell];

	struct HitStruct {
		ObjectClass * Object;
		int Distance;
	};
	std::vector<HitStruct> hits;

	/*
	 * Objects flying over the blast. A pinpoint blast on an object under the Iron Curtain
	 * spends itself there and spares everything else.
	 */
	if (Map.Get_Height_GL(cell) < coord.Z) {
		DynamicVectorClass<FootClass *> flyers;
		for (int index = 0; index < Aircraft.Count(); index++) {
			flyers.Add(Aircraft[index]);
		}
		for (int index = 0; index < Infantry.Count(); index++) {
			if (Infantry[index]->Class->IsJumpJet) flyers.Add(Infantry[index]);
		}
		for (int index = 0; index < Units.Count(); index++) {
			if (Units[index]->Class->IsJellyfish || Units[index]->In_Air()) flyers.Add(Units[index]);
		}
		for (int index = 0; index < flyers.Count(); index++) {
			FootClass * flyer = flyers[index];
			if (flyer->IsActive && flyer->IsDown && flyer->Strength > 0 && flyer->In_Air()) {
				distance = (coord - flyer->PositionCoord).Length();
				if (distance <= range) {
					if (pinpoint && distance < 85 && flyer->Is_Iron_Curtained() && !flyer->IsForceShielded) {
						curtain_hit = true;
					}
					hits.push_back({flyer, distance});
				}
			}
		}
	}

	bool isbridge = false;
	if (cellptr->IsUnderBridge && coord.Z > Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT / 2) {
		isbridge = true;
	}

	int const radius = std::clamp((int)(warhead->CellSpread + 0.99f), 0, CELL_SPREAD_MAX);
	int const count = Cell_Spread_Count(radius);
	for (int index = 0; index < count; index++) {
		Cell const where = cell + Cell_Spread_Offset(index);
		if (!Map.In_Radar(where)) continue;
		CellClass * cptr = &Map[where];

		/*
		 * Ore and walls in reach take their share of the blast.
		 */
		if (cptr->Overlay != OVERLAY_NONE) {
			OverlayTypeClass const * optr = OverlayTypes[cptr->Overlay];
			if (optr->IsChainReaction && (!optr->IsTiberium || warhead->IsTiberiumDestroyer) && dochainreaction) {
				cptr->Reduce_Tiberium(strength / 10);
			}
			if (optr->IsWall) {
				if (warhead->IsWallAbsoluteDestroyer) {
					cptr->Reduce_Wall(-1);
				} else if (warhead->IsWallDestroyer || (warhead->IsWoodDestroyer && optr->Armor == ARMOR_WOOD)) {
					cptr->Reduce_Wall(strength);
				}
			}
			if (cptr->Overlay == OVERLAY_NONE) {
				TechnoClass::Remove_Target(cptr);
			}
		}

		/*
		 * A structure is measured from the cell it is found in, so a large one is struck once
		 * for each of its cells in reach. The firer is spared unless its type is DamageSelf.
		 * Harvesters are spared in a scenario that protects them.
		 */
		Coord const cell_coord = cptr->Center_Coord();
		for (object = cptr->Cell_Occupier(isbridge); object != NULL; object = object->Next) {
			if (object == source && !source->TClass->IsDamageSelf) continue;
			if (!object->IsActive) continue;
			if (object->RTTI == RTTI_UNIT && Scen->Special.IsHarvesterImmune && Rule->HarvesterUnit.Is_In_List((UnitTypeClass *)object->Class_Of())) continue;

			if (object->RTTI == RTTI_BUILDING) {
				if (index != 0) {
					distance = (cell_coord - coord).Length();
				} else if (coord.Z - cell_coord.Z > 2 * LEVEL_LEPTON_H) {
					distance = (cell_coord - coord).Length() - 2 * LEVEL_LEPTON_H;
				} else {
					distance = 0;
				}
			} else {
				distance = (object->Target_Coord() - coord).Length();
			}

			if (pinpoint && index == 0 && object->Is_Techno() && ((TechnoClass *)object)->Is_Iron_Curtained() && !((TechnoClass *)object)->IsForceShielded && distance < 85) {
				curtain_hit = true;
			}
			hits.push_back({object, distance});
		}

		if (cptr->Overlay != OVERLAY_NONE && OverlayTypes[cptr->Overlay]->IsVeinholeMonster) {
			VeinholeMonsterClass * veinhole = VeinholeMonsterClass::Get_Monster_At(where);
			if (veinhole != NULL) {
				hits.push_back({veinhole, (veinhole->Target_Coord() - coord).Length()});
			}
		}
	}

	/*
	 * Deal the damage. An aircraft in the air counts as half as far away.
	 */
	HouseClass * const previous_source = DamageSourceHouse;
	if (sourcehouse != NULL) {
		DamageSourceHouse = sourcehouse;
	}
	for (HitStruct const & hit : hits) {
		object = hit.Object;
		object->IsToDamage = false;
		if (!object->IsActive) continue;
		if (object->RTTI == RTTI_BUILDING && ((BuildingClass *)object)->Class->IsInvisibleInGame) continue;
		if (curtain_hit && !(object->Is_Techno() && ((TechnoClass *)object)->Is_Iron_Curtained())) continue;

		distance = hit.Distance;
		if (object->RTTI == RTTI_AIRCRAFT && object->In_Air()) {
			distance /= 2;
		}
		if (object->Strength > 0 && object->IsDown && !object->IsInLimbo && distance <= range) {
			int damage = strength;
			if (warhead != Rule->IonStormWarhead || !object->Is_Foot() || ((FootClass *)object)->Team == NULL || !((FootClass *)object)->Team->Class->IsIonImmune) {
				object->Take_Damage(damage, distance, warhead, source);
			}
		}
	}
	DamageSourceHouse = previous_source;

	double rocking_force = std::min(strength * 0.01, 4.0);
	if (warhead->IsRocker && rocking_force > 0.3) {
		for (int x = cell.X - 3; x <= cell.X + 3; x++) {
			for (int y = cell.Y - 3; y <= cell.Y + 3; y++) {
				object = isbridge ? Map[Cell(x, y)].Cell_Occupier(true) : Map[Cell(x, y)].Cell_Occupier(false);

				while (object) {
					TechnoClass * techno = Dynamic_Cast<TechnoClass *>(object);
					if (techno != NULL) {
						if (Cell(x, y) == cell && source) {
							Coord tcoord = techno->PositionCoord;

							Coord scoord = source->PositionCoord;
							TPoint3D<float> rockdirf((float)(scoord.X - tcoord.X), (float)(scoord.Y - tcoord.Y), (float)(scoord.Z - tcoord.Z));
							rockdirf = rockdirf.Normalize() * 10;

							tcoord += Coord(rockdirf.X, rockdirf.Y, rockdirf.Z);
							techno->Rock(tcoord, rocking_force);
						} else {
							techno->Rock(coord, rocking_force);
						}
					}
					object = object->Next;
				}
			}
		}
	}

	cellptr = &Map[cell];

	/*
	**	If there is a bridge at this location, then it may be destroyed by the
	**	combat damage.
	*/
	bool ion_cannon = warhead == Rule->IonCannonWarhead;
	if (Scen->Special.IsDestroyBridges && warhead->IsWallDestroyer) {

		IsometricTileType ittype1 = IsometricTileType(cellptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
		CellClass * bridge_deck = cellptr->Get_Bridge_Deck_CellClass();

		if (bridge_deck && bridge_deck->Is_Overlay_Bridge() ||
			ittype1 == IsometricTileTypeClass::BridgeMiddle1 + 0 || ittype1 == IsometricTileTypeClass::BridgeMiddle1 + 1 ||
			ittype1 == IsometricTileTypeClass::BridgeMiddle1 + 2 || ittype1 == IsometricTileTypeClass::BridgeMiddle1 + 3 ||
			ittype1 == IsometricTileTypeClass::BridgeMiddle2 + 0 || ittype1 == IsometricTileTypeClass::BridgeMiddle2 + 1 ||
			ittype1 == IsometricTileTypeClass::BridgeMiddle2 + 2 || ittype1 == IsometricTileTypeClass::BridgeMiddle2 + 3) {
			if (!cellptr->IsUnderBridge || coord.Z <= BRIDGE_LEPTON_HEIGHT + LEVEL_LEPTON_H * (cellptr->Height + 1) && coord.Z > BRIDGE_LEPTON_HEIGHT + LEVEL_LEPTON_H * (cellptr->Height - 2)) {
				if (warhead->IsWallDestroyer && (warhead == Rule->IonCannonWarhead || Random_Pick(1, Rule->BridgeStrength) < strength)) {
					bool bridge_damaged = false;
					int tries=4;
					do {
						bridge_damaged = Map.Damage_Bridge(cell);
						tries--;
						if (bridge_damaged) {
							break;
						}
					} while (ion_cannon && tries > 0);

					if (bridge_damaged) {
						TechnoClass::Remove_Target(cellptr);
					}
					Point2D point;
					TacticalMap->Coord_To_Pixel(coord, point);
					TacticalMap->Register_Dirty_Area(Rect(point.X - 128, point.Y - 128, 256, 256), false);
				}
			}
		}

		IsometricTileType ittype2 = IsometricTileType(cellptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);

		if (bridge_deck && bridge_deck->Is_Overlay_Rail_Bridge() ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 0 || ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 1 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 2 || ittype2 == IsometricTileTypeClass::BridgeMiddle1 + 3 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 0 || ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 1 ||
			ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 2 || ittype2 == IsometricTileTypeClass::BridgeMiddle2 + 3) {
			if (!cellptr->IsUnderBridge || coord.Z <= BRIDGE_LEPTON_HEIGHT + LEVEL_LEPTON_H * (cellptr->Height + 1) && coord.Z > BRIDGE_LEPTON_HEIGHT + LEVEL_LEPTON_H * (cellptr->Height - 2)) {
				if (warhead->IsWallDestroyer && (warhead == Rule->IonCannonWarhead || Random_Pick(1, Rule->BridgeStrength) < strength)) {
					bool bridge_damaged = false;
					int tries=4;
					do {
						bridge_damaged = Map.Damage_Bridge(cell);
						tries--;
						if (bridge_damaged) {
							break;
						}
					} while (ion_cannon && tries > 0);

					if (bridge_damaged) {
						TechnoClass::Remove_Target(cellptr);
					}
					Point2D point;
					TacticalMap->Coord_To_Pixel(coord, point);
					TacticalMap->Register_Dirty_Area(Rect(point.X - 96, point.Y - 96, 192, 192), false);
				}
			}
		}

		if (cellptr->Overlay >= OVERLAY_LOWBRIDGE_01 && cellptr->Overlay <= OVERLAY_LOWBRIDGE_26) {
			if (warhead == Rule->IonCannonWarhead || Random_Pick(1, Rule->BridgeStrength) < strength) {
				bool destroyed = Map.Damage_Low_Bridge(cell);
				Map.Damage_Low_Bridge(cell);
				if (destroyed) {
					TechnoClass::Remove_Target(cellptr);
				}
			}
		}
	}

	if (cellptr->Overlay != OVERLAY_NONE && OverlayTypes[cellptr->Overlay]->IsExplosive) {
		cellptr->Register_For_Redraw();
		cellptr->Overlay = OVERLAY_NONE;
		cellptr->Recalc_Attributes();
		Map.Update_Cell_Zone(cellptr->CellID);
		Map.Update_Cell_Subzones(cellptr->CellID);
		TechnoClass::Remove_Target(cellptr);

		new AnimClass(Rule->BarrelExplode, coord);
		Explosion_Damage(coord, Rule->AmmoCrateDamage, NULL, Rule->C4Warhead, true);
		for (int i = 0; i < Rule->BarrelDebris.Count(); i++) {
			if (Percent_Chance(15)) {
				new VoxelAnimClass(Rule->BarrelDebris[i], coord);
				break;
			}
		}
		if (Percent_Chance(25)) {
			ParticleSystemClass * psys = new ParticleSystemClass(Rule->BarrelParticle, coord);
			psys->Spawn_Held_Particle(coord, coord);
		}

		static FacingType _part_facings[] = { FACING_N, FACING_E, FACING_S, FACING_W };

		for (int f = 0; f < ARRAY_SIZE(_part_facings); f++) {
			Coord adjacent = Adjacent_Coord_With_Height(coord, _part_facings[f]);
			if (Map[adjacent].Overlay != OVERLAY_NONE && OverlayTypes[Map[adjacent].Overlay]->IsExplosive) {
				new AnimClass(AnimTypes[AnimTypeClass::From_Name("FIRE3")], adjacent, Random_Pick(1, 3) + 3);
			}
		}
	}

	if (strength > warhead->DeformThreshhold) {
		if (Percent_Chance((strength * 0.01) * warhead->Deform * 100.0) && !(Map[coord].IsUnderBridge && coord.Z >= BRIDGE_LEPTON_HEIGHT + Map.Get_Height_GL(coord))) {
			Map.Deform_Terrain(coord.As_Cell(), false);
		}
	}

	if (cellptr->Is_Tile_Destroyable_Cliff()) {
		if (Percent_Chance(Rule->CollapseChance)) {
			Map.Collapse_Cliff(cellptr);
		}
	}

	if (warhead->Particle) {
		ParticleSystemClass * psys;
		if (warhead->Particle->BehavesLike == PSYS_BEHAVIOR_GAS) {
			psys = GasSystem;
		} else {
			psys = new ParticleSystemClass(warhead->Particle, coord);
		}
		psys->Spawn_Held_Particle(coord, coord);
	}

	if ((warhead->IsWallDestroyer || warhead->IsFire) && !(Map[coord].IsUnderBridge && coord.Z >= BRIDGE_LEPTON_HEIGHT + Map.Get_Height_GL(coord))) {
		Map.DirtyIceCells.Clear();
		if (Map.Crack_Ice(cellptr, NULL)) {
			Map.Recalc_Ice_Cells();
		}
	}
	return(curtain_hit);
}


/***********************************************************************************************
 * Combat_Anim -- Determines explosion animation to play.                                      *
 *                                                                                             *
 *    This routine is called when a projectile impacts. This routine will determine what       *
 *    animation should be played.                                                              *
 *                                                                                             *
 * INPUT:   damage   -- The amount of damage this warhead possess (warhead size).              *
 *                                                                                             *
 *          warhead  -- The type of warhead.                                                   *
 *                                                                                             *
 *          land     -- The land type that this explosion is over. Sometimes, this makes       *
 *                      a difference (especially over water).                                  *
 *                                                                                             *
 * OUTPUT:  Returns with the animation to play. If no animation is to be played, then          *
 *          ANIM_NONE is returned.                                                             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/19/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
AnimTypeClass const * Combat_Anim(int damage, WarheadTypeClass const * warhead, LandType land, Coord const & coord)
{
	enum {
		DAMAGE_PER_SPLASH_ANIM = 35,
		DAMAGE_PER_EXPLOSION_ANIM = 25
	};

	/*
	**	For cases of no damage or invalid warhead, don't have any
	**	animation effect at all.
	*/
	if (damage == 0 || warhead == NULL) {
		return(NULL);
	}

	if (land == LAND_WATER && warhead->IsConventional && !(Map[coord].IsUnderBridge && coord.Z >= BRIDGE_LEPTON_HEIGHT + Map.Get_Height_GL(coord))) {
		if (Rule->SplashList.Count()) {
			int val = std::min(damage, DAMAGE_PER_SPLASH_ANIM * Rule->SplashList.Count() - 1);
			return(Rule->SplashList[val / DAMAGE_PER_SPLASH_ANIM]);
		}
		return(NULL);
	}

	if (warhead == Rule->LightningWarhead) {
		return(Rule->WeatherConBoltExplosion);
	}

	if (warhead->ExplosionSet.Count()) {
		if (warhead->IsEMEffect) {
			return(warhead->ExplosionSet[Random_Pick(0, warhead->ExplosionSet.Count() - 1)]);
		}
		int val = std::min(damage, DAMAGE_PER_EXPLOSION_ANIM * warhead->ExplosionSet.Count() - 1);
		return(warhead->ExplosionSet[val / DAMAGE_PER_EXPLOSION_ANIM]);
	}

	return(NULL);
}


/// <summary>
/// Creates the lighting flash of an explosion.
/// This routine is called alongside the explosion animation so that a bright warhead
/// briefly lights up the terrain around the blast.
/// </summary>
/// <param name="coord">The coordinate the blast is centered upon.</param>
/// <param name="damage">The strength of the blast, which governs the size of the flash.</param>
/// <param name="forced">Should the flash occur even if the warhead is not a bright one?</param>
void Combat_Lighting(Coord coord, int damage, WarheadTypeClass const * warhead, bool forced)
{
	if (forced || warhead != NULL && warhead->IsBright) {
		damage *= 64;
		int size = damage >> 8;
		if (size >= 63) {
			size = 63;
		} else if (size <= 21) {
			size = 21;
		}

		new SpotLightClass(coord, size);
	}
}


/***********************************************************************************************
 * Wide_Area_Damage -- Apply wide area damage to the map.                                      *
 *                                                                                             *
 *    This routine will apply damage to a very wide area on the map. The damage will be        *
 *    spread out from the coordinate specified by the radius specified. The amount of damage   *
 *    will attenuate according to the distance from center.                                    *
 *                                                                                             *
 * INPUT:   coord    -- The coordinate that the explosion damage will center about.            *
 *                                                                                             *
 *          radius   -- The radius of the explosion.                                           *
 *                                                                                             *
 *          damage   -- The amount of damage to apply at the center location.                  *
 *                                                                                             *
 *          source   -- Pointer to the purpetrator of the damage (if any).                     *
 *                                                                                             *
 *          warhead  -- The type of warhead that is causing the damage.                        *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/26/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void Wide_Area_Damage(Coord const & coord, LEPTON radius, int rawdamage, TechnoClass * source, WarheadTypeClass const * warhead)
{
	if (warhead == NULL) return;

	int cell_radius = (radius + CELL_LEPTON_W-1) / CELL_LEPTON_W;
	Cell cell = coord.As_Cell();

	if (rawdamage > warhead->DeformThreshhold) {
		if (Percent_Chance((rawdamage * 0.01) * warhead->Deform * 100.0)) {
			Map.Deform_Terrain(coord.As_Cell(), false);
		}
	}

	for (int x = -cell_radius; x <= cell_radius; x++) {
		for (int y = -cell_radius; y <= cell_radius; y++) {
			int xpos = cell.X + x;
			int ypos = cell.Y + y;

			/*
			**	If the potential damage cell is outside of the map bounds,
			**	then don't process it. This unusual check method ensures that
			**	damage won't wrap from one side of the map to the other.
			*/
			if ((unsigned)xpos > MAP_CELL_W) {
				continue;
			}
			if ((unsigned)ypos > MAP_CELL_H) {
				continue;
			}
			Cell tcell = Cell(xpos, ypos);
			if (!Map.In_Radar(tcell)) continue;

			int dist_from_center = Distance(Coord(x+cell_radius, y+cell_radius), Coord(cell_radius, cell_radius));
			double damagemod = 1;
			if (dist_from_center > 0) {
				damagemod = 1.0 / ((double)cell_radius / dist_from_center);
			}
			Coord tcoord = tcell.As_Coord();
			tcoord.Z = Map.Get_Height_GL(tcoord.As_Cell());
			Explosion_Damage(tcoord, rawdamage * damagemod, source, warhead);
		}
	}
}


/// <summary>
/// Determines the pitch to launch a ballistic projectile at.
/// This is the routine the weapon code calls when it wants to lob something at a target.
/// It takes the solution from Calculate_Projectile_Angle and turns it into a pitch the launcher
/// can be aimed with, correcting the flat solution when it would send the shot skyward.
/// </summary>
/// <param name="high_arc">Should the shot be lobbed rather than fired flat?</param>
/// <param name="speed">The speed the projectile will be launched at.</param>
/// <param name="distance">The horizontal distance to the target.</param>
/// <param name="height">The height of the target relative to the launch point.</param>
/// <param name="gravity">The gravity that will act upon the projectile.</param>
/// <param name="pitch">Reference to the pitch to be filled in.</param>
/// <returns>bool; Was a firing solution found?</returns>
bool Calculate_Projectile_Pitch(bool high_arc, int speed, int distance, int height, double gravity, DirType & pitch)
{
	/*
	 * Try launching the projectile as requested.
	 */
	double angle;
	if (Calculate_Projectile_Angle(high_arc, speed, distance, height, gravity, angle)) {

		/*
		 * The high arc always goes upwards, so just return the angle.
		 */
		if (high_arc) {
			pitch = DirType(angle);
			return(true);
		}

		/*
		 * The low arc solution can also come back pointing upwards, which makes no sense for
		 * a flat shot. Solving again for a slightly greater distance reveals which way it
		 * points: a smaller angle means the solution is the wrong way round and is reversed.
		 */
		double test;
		Calculate_Projectile_Angle(false, speed, distance + 1, height, gravity, test);
		if (test < angle) {
			pitch = DirType(-angle);
		} else {
			pitch = DirType(angle);
		}
		return(true);
	}
	return(false);
}


/// <summary>
/// Determines the elevation angle for a ballistic shot.
/// This routine solves the projectile equation for the angle that will drop the shot onto
/// the target. A reachable target usually has two answers -- a flat fast one and a lobbed
/// one -- and the caller says which of them it wants.
/// </summary>
/// <param name="high_arc">Should the lobbed solution be returned rather than the flat one?</param>
/// <param name="speed">The speed the projectile will be launched at.</param>
/// <param name="distance">The horizontal distance to the target.</param>
/// <param name="height">The height of the target relative to the launch point.</param>
/// <param name="gravity">The gravity that will act upon the projectile.</param>
/// <param name="angle">Reference to the angle, in radians, to be filled in.</param>
/// <returns>bool; Was a firing solution found?</returns>
bool Calculate_Projectile_Angle(bool high_arc, int speed, int distance, int height, double gravity, double & angle)
{
	if ((double)distance == 0.0) {
		distance = 0;
	}

	double dx = (double)distance;
	double dy = (double)height;
	double v = (double)speed;

	double dy2 = dy * dy;
	double vsq = v * v;
	double base = vsq - (dy * gravity);
	double g2 = gravity * gravity;
	double dx2 = dx * dx;

	/*
	 * Check if the equation has solutions.
	 */
	double value = (v * v * vsq)
				 - (2 * vsq * dy * gravity)
				 - (g2 * dx2);

	/*
	 * Ensure we're not trying to divide by zero.
	 */
	double denominator = ((dy2 / dx2) + 1.0) * 2;

	if (value < 0.0 || denominator == 0.0) {
		return(false); /// No valid trajectory
	}

	double numerator = !high_arc ? base + std::sqrt(value)
								  : base - std::sqrt(value);

	value = numerator / denominator;

	if (value >= 0.0) {
		/// Calculate the angle
		angle = std::acos(std::sqrt(value) / v);
		return(true);
	}
	return(false);
}


/// <summary>
/// Determines the launch speed needed to cover a range.
/// This routine is used by artillery style weapons to pick a muzzle velocity that will
/// carry the shot out as far as the target sits.
/// </summary>
/// <param name="range">The distance the projectile must cover.</param>
/// <param name="gravity">The gravity that will act upon the projectile.</param>
/// <returns>Returns with the speed to launch the projectile at.</returns>
MPHType Calculate_Projectile_Speed(int range, double gravity)
{
	return((MPHType)(int)std::sqrt((double)range * gravity * 1.2));
}


/// <summary>
/// Can a ballistic projectile reach the target at all?
/// This routine is used before a lobbed weapon is allowed to fire so that the object does
/// not line up a shot that gravity will never let it complete.
/// </summary>
/// <param name="speed">The speed the projectile would be launched at.</param>
/// <param name="distance">The horizontal distance to the target.</param>
/// <param name="height">The height of the target relative to the launch point.</param>
/// <param name="gravity">The gravity that will act upon the projectile.</param>
/// <returns>bool; Does a firing solution exist?</returns>
bool Is_Projectile_Trajectory_Valid(int speed, int distance, int height, double gravity)
{
	double v = (double)speed;
	double dx = (double)distance;
	if (dx == 0.0) dx = 0.001; /// Avoid division by zero
	double dy = (double)height;

	double y2 = dy * dy;
	double vsq = v * v;
	double g2 = gravity * gravity;
	double x2 = dx * dx;

	/*
	 * Check if the equation has solutions.
	 */
	double discriminant = (v * v * vsq)
						- (2 * vsq * dy * gravity)
						- (g2 * x2);

	if (discriminant < 0.0) {
		return(false);
	}

	/*
	 * Ensure we're not trying to divide by zero.
	 */
	double denominator = 2 * ((y2 / x2) + 1.0);
	if (denominator == 0.0) {
		return(false); /// No valid trajectory
	}

	double base = vsq - (dy * gravity);

	if (((base + std::sqrt(discriminant)) / denominator) >= 0.0 ||
		((base - std::sqrt(discriminant)) / denominator) >= 0.0) {
		return(true); /// Valid trajectory exists
	}
	return(false); /// No valid trajectory
}


/// <summary>
/// Fetches the Z adjustment to display an explosion with.
/// Explosion animations are nudged toward the viewer so that the blast draws over the
/// object it is consuming instead of disappearing behind it.
/// </summary>
/// <returns>Returns with the Z adjustment to hand to the animation.</returns>
int Get_Explosion_Z(Coord coord)
{
	return(-15);
}


/// <summary>
/// Fetches the gravity that acts upon a floating projectile.
/// Bullets marked as floaters drift toward their target rather than plummeting, so the
/// firing and trajectory routines use this gentler pull in place of the rule gravity.
/// </summary>
/// <returns>Returns with the gravity to use for a floating projectile.</returns>
double Get_Floater_Gravity(void)
{
	return(Rule->Gravity * 0.5);
}
