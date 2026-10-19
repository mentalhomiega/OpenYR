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

/* $Header: /CounterStrike/SUPER.CPP 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : SUPER.CPP                                                    *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : 07/28/95                                                     *
 *                                                                                             *
 *                  Last Update : October 11, 1996 [JLB]                                       *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   SuperClass::AI -- Process the super weapon AI.                                            *
 *   SuperClass::Anim_Stage -- Fetches the animation stage for this super weapon.              *
 *   SuperClass::Discharged -- Handles discharged action for special super weapon.             *
 *   SuperClass::Enable -- Enable this super special weapon.                                   *
 *   SuperClass::Forced_Charge -- Force the super weapon to full charge state.                 *
 *   SuperClass::Impatient_Click -- Called when player clicks on unfinished super weapon.      *
 *   SuperClass::Recharge -- Starts the special super weapon recharging.                       *
 *   SuperClass::Remove -- Removes super weapon availability.                                  *
 *   SuperClass::SuperClass -- Constructor for special super weapon objects.                   *
 *   SuperClass::Suspend -- Suspend the charging of the super weapon.                          *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "super.h"

#include "_map.h"
#include "_rules.h"
#include "_weapon.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "crc.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ionblast.h"
#include "language/language.h"
#include "lstorm.h"
#include "mouse.h"
#include "psydom.h"
#include "revent.h"
#include "rules.h"
#include "savestream.h"
#include "side.h"
#include "sun.h"
#include "suprtype.h"
#include "swrules.h"
#include "swizzle.h"
#include "tracker.h"
#include "unit.h"
#include "vector.h"
#include "vox.h"
#include "weapon.h"

#include <algorithm>
#include <cstdlib>
#include <vector>


/// <summary>
/// Default constructor for the super weapon objects.
/// This routine creates a blank super weapon with no type and no owning house, and adds
/// it to the global super weapon list.
/// </summary>
SuperClass::SuperClass(void) :
	Class(NULL),
	House(NULL),
	Control(0),
	NeedsBuilding(true),
	IsPresent(false),
	IsOneTime(false),
	IsReady(false),
	IsSuspended(false),
	ChargeDrainState(SUSPENDED)
{
	SuperWeapons.Add(this);
}


/***********************************************************************************************
 * SuperClass::SuperClass -- Constructor for special super weapon objects.                     *
 *                                                                                             *
 *    This is the constructor for the super weapons.                                           *
 *                                                                                             *
 * INPUT:   recharge    -- The recharge delay time (in game frames).                           *
 *                                                                                             *
 *          charging    -- Voice to announce that the weapon is charging.                      *
 *                                                                                             *
 *          ready       -- Voice to announce that the weapon is fully charged.                 *
 *                                                                                             *
 *          impatient   -- Voice to announce current charging state.                           *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
SuperClass::SuperClass(SuperWeaponTypeClass * type, HouseClass * owner) :
	Class(type),
	House(owner),
	Control(0),
	OldStage(-1),
	NeedsBuilding(true),
	IsPresent(false),
	IsOneTime(false),
	IsReady(false),
	IsSuspended(false),
	ChargeDrainState(SUSPENDED)
{
	Create_ID();
	SuperWeapons.Add(this);
	AbstractTypePtrTracker.Add(this);
	HousePtrTracker.Add(this);
	AnimPtrTracker.Add(this);
}


/// <summary>
/// Destructor for the super weapon objects.
/// This routine unhooks the super weapon from the global super weapon list and from the
/// trackers that watch its weapon type and its owning house.
/// </summary>
SuperClass::~SuperClass(void)
{
	SuperWeapons.Delete(this);
	AbstractTypePtrTracker.Delete(this);
	HousePtrTracker.Delete(this);
	AnimPtrTracker.Delete(this);
}


/***********************************************************************************************
 * SuperClass::Suspend -- Suspend the charging of the super weapon.                            *
 *                                                                                             *
 *    This will temporarily put on hold the charging of the special weapon. This might be the  *
 *    result of insufficient power.                                                            *
 *                                                                                             *
 * INPUT:   on -- Should the weapon charging be suspended? Else, it will unsuspend.            *
 *                                                                                             *
 * OUTPUT:  Was the weapon suspend state changed?                                              *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SuperClass::Suspend(bool on)
{
	if (IsPresent && !IsOneTime && on != IsSuspended && NeedsBuilding) {
		if (!on && !Class->IsManualControl) {
			Control.Start();
		} else {
			Control.Stop();
		}
		IsSuspended = on;
		if (Class->UseChargeDrain) {
			if (on) {
				ChargeDrainState = SUSPENDED;
				House->Deactivate_Firestorm();
			} else {
				ChargeDrainState = CHARGING;
				Control = Get_Recharge_Time();
			}
		}
		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * SuperClass::Enable -- Enable this super special weapon.                                     *
 *                                                                                             *
 *    This routine is called when the special weapon needs to be activated. This is used for   *
 *    both the normal super weapons and the special one-time super weapons (from crates).      *
 *                                                                                             *
 * INPUT:   onetime  -- Is this a special one time super weapon?                               *
 *                                                                                             *
 *          player   -- Is this weapon for the player? If true, then there might be a voice    *
 *                      announcement of this weapon's availability.                            *
 *                                                                                             *
 *          quiet    -- Request that the weapon start in suspended state (quiet mode).         *
 *                                                                                             *
 * OUTPUT:  Was the special super weapon enabled? Failure might indicate that the weapon was   *
 *          already available.                                                                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SuperClass::Enable(bool onetime, bool player, bool quiet)
{
	if (!IsPresent) {
		IsPresent = true;
		IsOneTime = onetime;
		if (IsVirtualCharging) {
			IsVirtualCharging = false;
			if (!onetime) {
				if (quiet) Suspend(true);
				return(false);
			}
		}
		bool retval = false;
		if (Class->IsManualControl) {
			OldStage = -1;
			Control = Get_Recharge_Time();
			Control.Stop();
		} else {
			retval = Recharge(player && !quiet);
		}
		if (IsOneTime) Forced_Charge(player);
		Note_Enabled(player && !quiet);
		if (quiet) Suspend(true);
		return(retval);
	}
	return(false);
}


/***********************************************************************************************
 * SuperClass::Remove -- Removes super weapon availability.                                    *
 *                                                                                             *
 *    Call this routine when the super weapon should be removed because of some outside        *
 *    force. For one time special super weapons, they can never be removed except as the       *
 *    result of discharging them.                                                              *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Was the special weapon removed and disabled?                                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SuperClass::Remove(void)
{
	if (IsPresent) {
		if (PreClickAnim != NULL) {
			Stop_Pre_Click_Anim();
			if (House == PlayerPtr && Map.IsTargettingMode != SUPER_NONE && Map.IsTargettingMode == Follow_Up()) {
				Map.IsTargettingMode = SUPER_NONE;
			}
		}
		if (Class->Keys.VirtualCharge && !IsOneTime && !Class->UseChargeDrain) {
			IsVirtualCharging = true;
		} else {
			IsReady = false;
		}
		IsPresent = false;
		if (Class->UseChargeDrain) {
			House->Deactivate_Firestorm();
		}
		return(true);
	}
	return(false);
}


/// <summary>
/// The frames this weapon takes to charge: the time a trigger action gave it, or else the time its
/// type gives.
/// </summary>
int SuperClass::Get_Recharge_Time(void) const
{
	return(CustomRechargeTime != -1 ? CustomRechargeTime : Class->RechargeTime);
}


/// <summary>
/// Gives this weapon a charge time of its own, which applies from the next time it charges.
/// </summary>
/// <param name="frames">The charge time in frames.</param>
void SuperClass::Set_Recharge_Time(int frames)
{
	CustomRechargeTime = frames;
}


/// <summary>
/// Puts the charge time back to the one the weapon's type gives.
/// </summary>
void SuperClass::Reset_Recharge_Time(void)
{
	CustomRechargeTime = -1;
}


/// <summary>
/// Sets how charged a present weapon is, as SuperClass::SetCharge (0x6CC1E0) does. At 100 percent
/// one frame is left, so the weapon is ready on the next frame.
/// </summary>
/// <param name="percent">How charged the weapon is, from 0 to 100.</param>
void SuperClass::Set_Charge(int percent)
{
	if (!IsPresent || percent < 0 || percent > 100) {
		return;
	}

	int const total = Get_Recharge_Time();
	// The charged share is percent times the float32 0.01 (0x7EAAE0), truncated.
	int const left = total - static_cast<int>(static_cast<double>(percent) * 0.01f * total);
	if (left == 0) {
		IsReady = true;
	}
	OldStage = -1;
	Control.Start();
	Control = left;
}


/// <summary>
/// Starts a present weapon's charge over from the beginning, as SuperClass::Reset (0x6CE0B0)
/// does. A suspended weapon also starts over, but its countdown stays stopped until it resumes.
/// </summary>
void SuperClass::Reset(void)
{
	if (IsPresent) {
		IsReady = false;
		if (IsSuspended) {
			// The countdown restarts at the full recharge time, which is an empty charge, and stays stopped.
			Control = Get_Recharge_Time();
			Control.Stop();
		} else {
			Recharge(false);
		}
	}
}


/// <summary>
/// Announces that a super weapon is ready. With an announcer file this is the Yuri's Revenge
/// line for its behavior (SuperClass::AI, 0x6CBCA0), and paradrops share the reinforcements
/// line; without one it is the weapon's RechargeVoice.
/// </summary>
static void Speak_Ready(SuperWeaponTypeClass const * type)
{
	// EVA.Ready names the line to play, and "none" plays nothing.
	if (!type->Keys.EvaReady.empty()) {
		if (!SuperRules::Same(type->Keys.EvaReady, "none")) {
			Speak_Eva(type->Keys.EvaReady.c_str());
		}
		return;
	}
	if (!Is_Eva_Loaded()) {
		Speak(type->VoxRecharge);
		return;
	}
	switch (type->Type) {
		case SUPER_MULTI_MISSILE: Speak_Eva("EVA_NuclearMissileReady"); break;
		case SUPER_IRON_CURTAIN: Speak_Eva("EVA_IronCurtainReady"); break;
		case SUPER_FORCE_SHIELD: Speak_Eva("EVA_ForceShieldReady"); break;
		case SUPER_LIGHTNING_STORM: Speak_Eva("EVA_LightningStormReady"); break;
		case SUPER_PSYCHIC_DOMINATOR: Speak_Eva("EVA_PsychicDominatorReady"); break;
		case SUPER_CHRONOSPHERE: Speak_Eva("EVA_ChronosphereReady"); break;
		case SUPER_PARA_DROP:
		case SUPER_AMER_PARA_DROP: Speak_Eva("EVA_ReinforcementsReady"); break;
		case SUPER_SPY_PLANE: Speak_Eva("EVA_SpyPlaneReady"); break;
		case SUPER_GENETIC_CONVERTER: Speak_Eva("EVA_GeneticMutatorReady"); break;
		case SUPER_PSYCHIC_REVEAL: Speak_Eva("EVA_PsychicRevealReady"); break;
		default: break;
	}
}


/***********************************************************************************************
 * SuperClass::Recharge -- Starts the special super weapon recharging.                         *
 *                                                                                             *
 *    This routine is called when the special weapon is allowed to recharge. Suspension will   *
 *    be disabled and the animation process will begin.                                        *
 *                                                                                             *
 * INPUT:   player   -- Is this for a player owned super weapon? If so, then a voice           *
 *                      announcement might be in order.                                        *
 *                                                                                             *
 * OUTPUT:  Was the super weapon begun charging up?                                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SuperClass::Recharge(bool player)
{
	if (IsPresent && !IsReady && !IsSuspended) {
		OldStage = -1;
		Control.Start();
		Control = Get_Recharge_Time();

		if (Class->UseChargeDrain) {
			ChargeDrainState = CHARGING;
		}

#ifdef _DEBUG
		if (Special.IsSpeedBuild) {
			Control = 1;
		}
#endif

		if (player) {
			Speak(Class->VoxCharging);
		}
		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * Superclass::Discharged -- Handles discharged action for special super weapon.               *
 *                                                                                             *
 *    This routine should be called when the special super weapon has been discharged. The     *
 *    weapon will either begin charging anew or will be removed entirely -- depends on the     *
 *    one time flag for the weapon.                                                            *
 *                                                                                             *
 * INPUT:   player   -- Is this special weapon for the player? If so, then there might be a    *
 *                      voice announcement.                                                    *
 *                                                                                             *
 * OUTPUT:  Should the sidebar be reprocessed because the special weapon has been eliminated?  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SuperClass::Discharged(bool player, Cell const & cell)
{
	if (Class->UseChargeDrain) {
		if (House->Is_Human_Player() || Class->Type == SUPER_BATTERY) {
			if (ChargeDrainState == FIRESTORM_ON) {
				ChargeDrainState = READY;
				Control = Get_Recharge_Time() - Control.Value() / Drain_Ratio();
				Deactivate_Firestorm(0, player);
				return(false);
			}
			if (ChargeDrainState == READY) {
				if (!Check_Launch(cell, player)) {
					return(false);
				}
				ChargeDrainState = FIRESTORM_ON;
				Control = (Get_Recharge_Time() - Control.Value()) * Drain_Ratio();
				Begin_Launch(cell, player);
				return(false);
			}
		} else {
			if (House->FirestormDefenseActivated) {
				Deactivate_Firestorm(0, player);
			} else {
				Place(cell, player);
			}
		}
		return(false);
	}

	/*
	 * A PostClick weapon fires charged or not, once the PreClick weapon it completes has
	 * picked its cells; that weapon is the one used up (HouseClass::Fire_SW, 0x4FAE50).
	 */
	if (Class->IsPostClick) {
		SuperClass * first = Pre_Dependent();
		if (first == NULL || !first->IsPresent || !first->IsReady) {
			return(false);
		}
		if (!Check_Launch(cell, player)) {
			return(false);
		}
		ChronoSource = first->ChronoSource;
		Place(cell, player);
		first->Stop_Pre_Click_Anim();
		first->IsReady = false;
		if (first->IsOneTime) {
			first->IsOneTime = false;
			return(first->Remove());
		}
		first->OldStage = -1;
		first->Control = first->Get_Recharge_Time();
		if (first->IsSuspended || first->Class->IsManualControl) {
			first->Control.Stop();
		}
		return(false);
	}

	if (Control.Is_Active() && IsPresent && IsReady) {

		// One storm and one dominator blast at a time; a refused shot keeps its charge (SuperClass::ClickFire).
		if (Class->Type == SUPER_LIGHTNING_STORM && LightningStormClass::Is_Active_Or_Pending()) {
			if (player) {
				if (Class->Keys.MessageAbort.empty()) {
					LightningStormClass::Print_Refusal();
				} else {
					Show_Message(Class->Keys.MessageAbort);
				}
			}
			return(false);
		}
		if (Class->Type == SUPER_PSYCHIC_DOMINATOR && PsychicDominatorClass::Is_Active()) {
			if (player) {
				if (Class->Keys.MessageAbort.empty()) {
					PsychicDominatorClass::Print_Refusal();
				} else {
					Show_Message(Class->Keys.MessageAbort);
				}
			}
			return(false);
		}

		if (!Check_Launch(cell, player)) {
			return(false);
		}
		Begin_Launch(cell, player);
		if (Class->IsPreClick) {
			return(false);
		}
		IsReady = false;
		if (IsOneTime) {
			IsOneTime = false;
			return(Remove());
		} else {
			if (Class->IsManualControl) {
				Control = Get_Recharge_Time();
				OldStage = -1;
				Control.Stop();
			} else {
				Recharge(player);
			}
		}
	}
	return(false);
}


/***********************************************************************************************
 * SuperClass::AI -- Process the super weapon AI.                                              *
 *                                                                                             *
 *    This routine will process the charge up AI for this super weapon object. If the weapon   *
 *    has advanced far enough to change any sidebar graphic that might represent it, then      *
 *    "true" will be returned. Use this return value to intelligently update the sidebar.      *
 *                                                                                             *
 * INPUT:   player   -- Is this for the player? If it is and the weapon is now fully charged,  *
 *                      then this fully charged state will be announced to the player.         *
 *                                                                                             *
 * OUTPUT:  Was the weapon's state changed such that a sidebar graphic update will be          *
 *          necessary?                                                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SuperClass::AI(bool player)
{
	if (IsSpecialSoundPending && SpecialSoundTimer == 0) {
		IsSpecialSoundPending = false;
		Sound_Effect(Class->SpecialSound, SpecialSoundCoord);
	}

	// The chronosphere's marker shows only while its owner, the player, aims the warp.
	if (PreClickAnim != NULL && (!player || Map.IsTargettingMode == SUPER_NONE || Map.IsTargettingMode != Follow_Up())) {
		PreClickAnim->Make_Invisible();
	}

	if (IsVirtualCharging && !IsPresent) {
		if (!IsReady && !IsSuspended && Control.Is_Active() && Control == 0) {
			IsReady = true;
		}
		return(false);
	}

	if (IsPresent && (!IsReady || Class->UseChargeDrain) && !IsSuspended) {
		if (!Control.Is_Active()) {
			if (OldStage != -1) {
				OldStage = -1;
				return(true);
			}
		} else {
			if (Control == 0) {
				if (Class->UseChargeDrain) {
					if (ChargeDrainState == FIRESTORM_ON) {
						ChargeDrainState = CHARGING;
						Deactivate_Firestorm(0, player);
						Control = Get_Recharge_Time();
					} else {
						ChargeDrainState = READY;
						IsReady = true;
					}
					return(true);
				} else {
					IsReady = true;
					ReadyFrame = Frame;
					if (player) {
						Speak_Ready(Class);
						Show_Message(Class->Keys.MessageReady);
					}
					return(true);
				}
			} else {
				if (Anim_Stage() != OldStage) {
					OldStage = Anim_Stage();
					return(true);
				}
			}
		}
	}
	return(false);
}


/***********************************************************************************************
 * SuperClass::Anim_Stage -- Fetches the animation stage for this super weapon.                *
 *                                                                                             *
 *    This will return the current animation stage for this super weapon. The value will be    *
 *    between zero (uncharged) to ANIMATION_STAGES (fully charged). Use this value to render   *
 *    the appropriate graphic on the sidebar.                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the current animation stage for this special super weapon powerup.    *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *   10/11/1996 JLB : Doesn't show complete until really complete.                             *
 *=============================================================================================*/
int SuperClass::Anim_Stage(void) const
{
	if (IsPresent) {
		int stage;
		if (Class->UseChargeDrain) {
			if (ChargeDrainState == FIRESTORM_ON) {
				stage = ANIMATION_STAGES * (1-double(Get_Recharge_Time()*Drain_Ratio()-Control.Value()) / (Get_Recharge_Time()*Drain_Ratio()));
			} else {
				stage = ANIMATION_STAGES * (double(Get_Recharge_Time()-Control.Value()) / Get_Recharge_Time());
			}
		} else {
			if (IsReady) {
				return(ANIMATION_STAGES);
			}
			stage = ANIMATION_STAGES * (double(Get_Recharge_Time()-Control.Value()) / Get_Recharge_Time());
		}
		stage = std::min(stage, ANIMATION_STAGES-1);
		return(stage);
	}
	return(0);
}


/***********************************************************************************************
 * SuperClass::Impatient_Click -- Called when player clicks on unfinished super weapon.        *
 *                                                                                             *
 *    This routine is called when the player clicks on the super weapon icon on the sidebar    *
 *    when the super weapon is not ready yet. This results in a voice message feedback to the  *
 *    player.                                                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void SuperClass::Impatient_Click(void) const
{
	if (!Control.Is_Active()) {
		Speak(Class->VoxSuspend);
	} else if (!Class->Keys.EvaImpatient.empty()) {
		if (!SuperRules::Same(Class->Keys.EvaImpatient, "none")) {
			Speak_Eva(Class->Keys.EvaImpatient.c_str());
		}
	} else {
		Speak(Class->VoxImpatient);
	}
}


/***********************************************************************************************
 * SuperClass::Forced_Charge -- Force the super weapon to full charge state.                   *
 *                                                                                             *
 *    This routine will force the special weapon to full charge state. Call it when the weapon *
 *    needs to be instantly charged. The airstrike (when it first becomes available) is a      *
 *    good example.                                                                            *
 *                                                                                             *
 * INPUT:   player   -- Is this for the player? If true, then the full charge state will be    *
 *                      announced.                                                             *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void SuperClass::Forced_Charge(bool player)
{
	if (IsPresent) {
		IsReady = true;
		Control.Start();
		Control = 0;
//		IsSuspended = false;
		if (player) {
			Speak_Ready(Class);
			Show_Message(Class->Keys.MessageReady);
		}
		if (Class->UseChargeDrain) {
			ChargeDrainState = READY;
		}
	}
}


/// <summary>
/// Does the super weapon need power in order to charge?
/// </summary>
/// <returns>bool; Is this super weapon dependent upon the owner's power supply?</returns>
bool SuperClass::Is_Powered(void) const
{
	return(Class->IsPowered);
}


/// <summary>
/// Fetches the status text for the super weapon.
/// This routine is used by the sidebar to caption the super weapon's cameo with a
/// description of what it is currently up to.
/// </summary>
/// <returns>Returns with a pointer to the text to display, or NULL if the weapon has
/// nothing worth saying.</returns>
char const * SuperClass::State_String(void) const
{
	if (!IsSuspended) {
		if (Class->UseChargeDrain) {
			switch (ChargeDrainState) {
				case CHARGING:
					return(Fetch_String(TXT_CHARGING));
				case READY:
					return(Fetch_String(TXT_READY));
				case FIRESTORM_ON:
					return(Fetch_String(TXT_FIRESTORM_ON));
				default:
					return(NULL);
			}
		}
		if (Class->Type == SUPER_HUNTER_SEEKER) {
			if (IsReady) {
				return(Fetch_String(TXT_RELEASE_THE_HOUNDS));
			} else {
				return(NULL);
			}
		}
		if (IsReady) {
			return(Fetch_String(TXT_READY));
		} else {
			return(NULL);
		}
	} else {
		return(Fetch_String(TXT_HOLD));
	}
}


/// <summary>
/// Can the super weapon be fired right now?
/// This routine is used to decide whether clicking the cameo should arm the targeting
/// cursor. A suspended weapon can never be fired, no matter how charged it is.
/// </summary>
/// <returns>bool; Is the super weapon ready to be used?</returns>
bool SuperClass::Can_Place(void) const
{
	if (!IsSuspended) {
		if (Class->UseChargeDrain) {
			return(ChargeDrainState != CHARGING ? true : false);
		}
		return(IsReady && IsPresent);
	}
	return(false);
}


/// <summary>
/// Puts the objects on the cell and the eight around it under the Iron Curtain, after the
/// invoke animation starts over the cell, as the IronCurtain case of SuperClass::Launch
/// (0x6CC390) does. On a bridge cell, only the objects on the bridge are covered.
/// </summary>
void SuperClass::Iron_Curtain(Cell const & cell) const
{
	Coord coord = Map[cell].Center_Coord();
	if (Map[cell].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	coord.Z += 5;
	if (Rule->IronCurtainInvokeAnim != NULL && Class->Keys.Animation == NULL) {
		new AnimClass(Rule->IronCurtainInvokeAnim, coord);
	}
	Submit_Radar_Event(RADAREVENT_SUPERWEAPON, cell);

	/*
	 * Gather everything first: the curtain kills infantry, which must not upset the walk
	 * through the cell's object list.
	 */
	// SW.Range and SW.AffectsHouse widen or narrow the stock 3 by 3 block of cells and every house.
	SuperRules::Area area = Class->Keys.Target_Area();
	if (!area.IsSet) {
		area.IsSet = true;
		area.IsBlock = true;
		area.Width = 3;
		area.Height = 3;
	}
	int const reach = SuperRules::Area_Reach(area, 512);
	DynamicVectorClass<TechnoClass *> covered;
	for (int dy = -reach; dy <= reach; dy++) {
		for (int dx = -reach; dx <= reach; dx++) {
			if (!SuperRules::Area_Contains(area, dx, dy)) {
				continue;
			}
			Cell const where = cell + Cell(dx, dy);
			if (!Map.In_Radar(where)) {
				continue;
			}
			CellClass & cellptr = Map[where];
			for (ObjectClass * object = cellptr.Cell_Occupier(cellptr.IsUnderBridge); object != NULL; object = object->Next) {
				if (!object->Is_Techno()) {
					continue;
				}
				HouseClass const * owner = object->Owner_HouseClass();
				SuperRules::Occupant const kind = object->RTTI == RTTI_BUILDING ? SuperRules::Occupant::Building : (object->RTTI == RTTI_INFANTRY ? SuperRules::Occupant::Infantry : SuperRules::Occupant::Unit);
				if (owner != NULL && !SuperRules::House_Passes(Class->Keys.AffectsHouse, owner == House, House->Is_Ally(owner))) {
					continue;
				}
				if (!SuperRules::Target_Passes(Class->Keys.AffectsTarget, cellptr.Land_Type() == LAND_WATER, kind)) {
					continue;
				}
				covered.Add((TechnoClass *)object);
			}
		}
	}
	for (int index = 0; index < covered.Count(); index++) {
		TechnoClass * techno = covered[index];
		if (techno->IsActive && !techno->IsInLimbo) {
			techno->Iron_Curtain(Class->Keys.ProtectDuration >= 0 ? Class->Keys.ProtectDuration : Rule->IronCurtainDuration, House, false);
		}
	}
}


/// <summary>
/// Mutates the infantry at the cell, as the GeneticMutator case of SuperClass::Launch does.
/// With MutateExplosion, a 10000-point blast through MutateExplosionWarhead decides who is
/// caught; otherwise every infantryman on the cell and the eight around it is killed through
/// MutateWarhead. Either way the infantry die as the warhead's InfDeath says, and a mutated
/// infantryman becomes a brute of the firing house.
/// </summary>
void SuperClass::Genetic_Mutator(Cell const & cell) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	Coord coord = Map[cell].Center_Coord();
	if (Map[cell].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	if (Rule->IonBlast != NULL && keys.Animation == NULL) {
		new AnimClass(Rule->IonBlast, Coord(coord.X, coord.Y, coord.Z + 5));
	}
	if (keys.Sound == VOC_NONE) {
		Sound_Effect(Rule->GeneticMutatorActivateSound, coord);
	}
	Submit_Radar_Event(RADAREVENT_SUPERWEAPON, cell);

	// With MutateFriendly=no the blast spares infantry of the firing house and its allies.
	HouseClass const * spare = Rule->MutateFriendly ? NULL : House;

	bool const explosion = keys.MutateExplosion >= 0 ? keys.MutateExplosion > 0 : Rule->MutateExplosion;
	WarheadTypeClass const * warhead = keys.Warhead != NULL ? keys.Warhead : (explosion ? Rule->MutateExplosionWarhead : Rule->MutateWarhead);
	if (explosion) {
		Explosion_Damage(coord, keys.Damage >= 0 ? keys.Damage : 10000, NULL, warhead, false, House, spare);
		return;
	}

	// SW.Range sizes the area, SW.AffectsHouse chooses whose infantry it takes and SW.AffectsTarget limits it to land or water cells.
	SuperRules::Area const area = keys.Target_Area_Or_Block(3, 3);
	unsigned const cells = keys.AffectsTarget & (SuperRules::TARGET_LAND | SuperRules::TARGET_WATER);
	int const reach = SuperRules::Area_Reach(area, 512);
	DynamicVectorClass<InfantryClass *> victims;
	for (int dy = -reach; dy <= reach; dy++) {
		for (int dx = -reach; dx <= reach; dx++) {
			if (!SuperRules::Area_Contains(area, dx, dy)) continue;
			Cell const where = cell + Cell(dx, dy);
			if (!Map.In_Radar(where)) continue;
			CellClass & cellptr = Map[where];
			if (!SuperRules::Target_Passes(cells, cellptr.Land_Type() == LAND_WATER, SuperRules::Occupant::Infantry)) continue;
			for (ObjectClass * object = cellptr.Cell_Occupier(cellptr.IsUnderBridge); object != NULL; object = object->Next) {
				if (object->RTTI != RTTI_INFANTRY) continue;
				InfantryClass * infantry = (InfantryClass *)object;
				HouseClass const * owner = infantry->House;
				if (spare != NULL && spare->Is_Ally(owner)) continue;
				if (owner != NULL && !SuperRules::House_Passes(keys.AffectsHouse, owner == House, House->Is_Ally(owner))) continue;
				if ((keys.MutateIgnoreCyborg && infantry->Class->IsCyborg) || (keys.MutateIgnoreNotHuman && infantry->Class->IsNotHuman)) continue;
				victims.Add(infantry);
			}
		}
	}

	// A weapon that names its own damage or warhead lets the warhead's Verses decide, and infantry with Natural=yes die outright unless Mutate.KillNatural is off.
	bool const through_warhead = keys.Damage >= 0 || keys.Warhead != NULL;
	for (int index = 0; index < victims.Count(); index++) {
		InfantryClass * infantry = victims[index];
		if (infantry->IsActive && !infantry->IsInLimbo) {
			bool const outright = !through_warhead || (keys.MutateKillNatural && infantry->Class->IsNatural);
			int damage = outright || keys.Damage < 0 ? infantry->Class->MaxStrength : keys.Damage;
			infantry->Take_Damage(damage, 0, warhead, NULL, outright, false, House);
		}
	}
}


/// <summary>
/// Raises the force shield around the cell, as the ForceShield case of SuperClass::Launch
/// does: every structure of the firing house or its allies whose center is within
/// ForceShieldRadius cells is protected for ForceShieldDuration frames, and the firing house
/// loses its power for ForceShieldBlackoutDuration frames.
/// </summary>
void SuperClass::Force_Shield(Cell const & cell)
{
	Coord coord = Map[cell].Center_Coord();
	if (Map[cell].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	if (Rule->ForceShieldInvokeAnim != NULL) {
		new AnimClass(Rule->ForceShieldInvokeAnim, Coord(coord.X, coord.Y, coord.Z + 5));
	}

	// Protect.* replace the rules values, SW.Range the radius, and SW.AffectsHouse the houses covered (the owner and its allies without it).
	SuperWeaponKeys const & keys = Class->Keys;
	int const duration = keys.ProtectDuration >= 0 ? keys.ProtectDuration : Rule->ForceShieldDuration;
	int const outage = keys.ProtectPowerOutage >= 0 ? keys.ProtectPowerOutage : Rule->ForceShieldBlackoutDuration;
	int const fade = keys.ProtectFadeSoundTime >= 0 ? keys.ProtectFadeSoundTime : Rule->ForceShieldPlayFadeSoundTime;
	double const radius = (keys.AreaIsSet && !keys.AreaIsBlock) ? (keys.AreaRadius < 0.0f ? 100000.0 : (double)keys.AreaRadius) : (double)Rule->ForceShieldRadius;
	SpecialSoundTimer = duration - fade;
	IsSpecialSoundPending = Class->SpecialSound != VOC_NONE;
	SpecialSoundCoord = coord;
	Sound_Effect(Class->StartSound, coord);

	House->PowerBlackout = outage;
	House->IsPowerBlackout = true;
	House->RecalcPower = true;

	for (int index = Buildings.Count() - 1; index >= 0; index--) {
		BuildingClass * building = Buildings[index];
		bool const covered = keys.AffectsHouseSet ? SuperRules::House_Passes(keys.AffectsHouse, building->House == House, House->Is_Ally(building)) : House->Is_Ally(building);
		if (covered && (building->Center_Coord() - coord).Gamemd_Length() < radius * CELL_LEPTON_W) {
			building->Iron_Curtain(duration, House, true);
		}
	}
}


/// <summary>
/// Calls in the planes for a paradrop or spy plane shot, as the ParaDrop, AmerParaDrop and
/// SpyPlane cases of SuperClass::Launch (0x6CC390) do. A paradrop aimed at water lands at the
/// nearest dry ground. Each entry of the side's ParaDropInf list sends a PDPLANE carrying
/// ParaDropNum infantry of that type, and nothing comes when the two lists differ in length. A
/// spy plane shot sends one SPYP for each entry of AllyParaDropInf.
/// </summary>
void SuperClass::Paradrop(Cell const & cell) const
{
	SuperWeaponKeys const & keys = Class->Keys;
	if (Class->Type == SUPER_SPY_PLANE && keys.SpyPlaneType != NULL) {
		// SpyPlane.Type and SpyPlane.Count replace the stock one plane for each ally paradrop entry.
		for (int index = 0; index < keys.SpyPlaneCount; index++) {
			House->Send_Plane(keys.SpyPlaneType, MISSION_SPYPLANE_APPROACH, cell);
		}
		return;
	}
	if (Class->Type == SUPER_SPY_PLANE) {
		AircraftType const plane = AircraftTypeClass::From_Name("SPYP");
		if (plane != AIRCRAFT_NONE && Rule->AllyParaDropInf.Count() == Rule->AllyParaDropNum.Count()) {
			for (int index = 0; index < Rule->AllyParaDropInf.Count(); index++) {
				House->Send_Plane(AircraftTypes[plane], MISSION_SPYPLANE_APPROACH, cell);
			}
		}
		return;
	}

	Cell target = cell;
	if (Map[cell].Land_Type() == LAND_WATER) {
		Cell const nearby = Map.Nearby_Location(cell, SPEED_FOOT);
		if (nearby != CELL_NONE && Map[nearby].Land_Type() != LAND_WATER) {
			target = nearby;
		}
	}

	// ParaDrop.* keys from the weapon take over: the country's own set first, then the side's, then the plain one.
	{
		SuperWeaponKeys::ParaGroup const * group = NULL;
		int const planning = House->Planning_Side();
		SideClass const * side = (planning >= 0 && planning < Sides.Count()) ? Sides[planning] : NULL;
		for (SuperWeaponKeys::ParaGroup const & candidate : keys.ParaByOwner) {
			if (SuperRules::Same(candidate.Owner, House->Class->Name())) {
				group = &candidate;
				break;
			}
		}
		for (size_t pass = 0; group == NULL && side != NULL && pass < keys.ParaByOwner.size(); pass++) {
			if (SuperRules::Same(keys.ParaByOwner[pass].Owner, side->Name())) {
				group = &keys.ParaByOwner[pass];
			}
		}
		if (group == NULL && keys.ParaDefault.IsSet) {
			group = &keys.ParaDefault;
		}
		if (group != NULL && group->Types.Count() > 0 && group->Types.Count() == group->Num.Count()) {
			AircraftTypeClass const * aircraft = group->Aircraft != NULL ? group->Aircraft : keys.ParaDefault.Aircraft;
			if (aircraft == NULL) {
				AircraftType const stock = AircraftTypeClass::From_Name("PDPLANE");
				aircraft = stock != AIRCRAFT_NONE ? AircraftTypes[stock] : NULL;
			}
			for (int plane_number = 0; aircraft != NULL && plane_number < keys.ParaDropCount; plane_number++) {
				AircraftClass * plane = NULL;
				for (int index = 0; index < group->Types.Count(); index++) {
					TechnoTypeClass const * type = group->Types[index];
					if (type == NULL || type->Fetch_RTTI() != RTTI_INFANTRYTYPE) {
						continue;
					}
					InfantryTypeClass const * trooper_type = static_cast<InfantryTypeClass const *>(type);
					if (plane == NULL) {
						plane = House->Send_Plane(aircraft, MISSION_PARADROP_APPROACH, target, trooper_type, group->Num[index]);
					} else {
						for (int count = 0; count < group->Num[index]; count++) {
							FootClass * trooper = static_cast<FootClass *>(trooper_type->Create_One_Of(House));
							if (trooper == NULL) {
								break;
							}
							plane->Cargo.Attach(trooper);
						}
					}
				}
			}
			return;
		}
	}

	TypeList<InfantryTypeClass const *> const * infantry = &Rule->AmerParaDropInf;
	TypeList<int> const * counts = &Rule->AmerParaDropNum;
	if (Class->Type == SUPER_PARA_DROP) {
		switch (House->Planning_Side()) {
			case SIDE_GDI:
				infantry = &Rule->AllyParaDropInf;
				counts = &Rule->AllyParaDropNum;
				break;

			case SIDE_THIRD:
				infantry = &Rule->YuriParaDropInf;
				counts = &Rule->YuriParaDropNum;
				break;

			default:
				infantry = &Rule->SovParaDropInf;
				counts = &Rule->SovParaDropNum;
				break;
		}
	}

	AircraftType const plane = AircraftTypeClass::From_Name("PDPLANE");
	if (plane == AIRCRAFT_NONE || infantry->Count() != counts->Count()) {
		return;
	}
	for (int index = 0; index < infantry->Count(); index++) {
		if ((*infantry)[index] != NULL) {
			House->Send_Plane(AircraftTypes[plane], MISSION_PARADROP_APPROACH, target, (*infantry)[index], (*counts)[index]);
		}
	}
}


/// <summary>
/// Fetches the owner's super weapon whose Type is this weapon's PreDependent, or NULL. A weapon
/// that names another follow-up with SW.PostDependent is passed over, and one that names this weapon is
/// preferred, so two chronospheres with their own warps do not pick each other's cells.
/// </summary>
SuperClass * SuperClass::Pre_Dependent(void) const
{
	if (Class->PreDependent == SUPER_NONE || House == NULL) {
		return(NULL);
	}
	SuperClass * first = NULL;
	SuperClass * named = NULL;
	for (int index = 0; index < House->SuperWeapon.Count(); index++) {
		SuperClass * super = House->SuperWeapon[index];
		if (super != NULL && super->Class != NULL && super->Class->Type == Class->PreDependent) {
			std::string const & follow = super->Class->Keys.PostDependent;
			bool const names_this = !follow.empty() && SuperRules::Same(follow, Class->Name());
			if (!follow.empty() && !names_this) {
				continue;
			}
			// Of several weapons with that behavior, the one that is present and ready is the one that picked the cells.
			if (super->IsPresent && super->IsReady) {
				if (names_this) {
					return(super);
				}
				if (named == NULL) {
					named = super;
				}
			}
			if (first == NULL) {
				first = super;
			}
		}
	}
	return(named != NULL ? named : first);
}


/// <summary>
/// Fetches the owner's index of the PostClick weapon that completes this weapon's shot, or
/// SUPER_NONE when the owner has none. SW.PostDependent names the weapon; without it the first
/// PostClick weapon that depends on this weapon's behavior completes the shot.
/// </summary>
SuperWeaponType SuperClass::Follow_Up(void) const
{
	if (House == NULL) {
		return(SUPER_NONE);
	}
	if (!Class->Keys.PostDependent.empty()) {
		SuperWeaponType const named = SuperWeaponTypeClass::From_Name(Class->Keys.PostDependent.c_str());
		if (named != SUPER_NONE && named < House->SuperWeapon.Count()) {
			return(named);
		}
	}
	for (int index = 0; index < House->SuperWeapon.Count(); index++) {
		SuperClass const * super = House->SuperWeapon[index];
		if (super != NULL && super->Class != NULL && super->Class->IsPostClick && super->Class->PreDependent == Class->Type) {
			return(SuperWeaponType(index));
		}
	}
	return(SUPER_NONE);
}


/// <summary>
/// Lets the chronosphere's marker finish its current loop and forgets it.
/// </summary>
void SuperClass::Stop_Pre_Click_Anim(void)
{
	if (PreClickAnim != NULL) {
		PreClickAnim->Loops = 0;
		PreClickAnim = NULL;
	}
}


static void Chrono_Kill(TechnoClass * techno, HouseClass * house = NULL)
{
	if (techno->IsActive && !techno->IsInLimbo) {
		int damage = techno->TClass->MaxStrength;
		techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, house);
	}
}


/// <summary>
/// Starts the chronosphere warp of a unit the chronosphere picked, to the coordinate (0x718260, 0x7187A0).
/// Whatever stands on the landing spot is destroyed, except the other units in moving; an
/// infantryman crushes only infantry on its own spot. The unit dies instead if something
/// there is under the Iron Curtain or the spot is off the map, and moves to the nearest free
/// cell if a structure or tree stands there. The unit then stays on its cell for ChronoDelay
/// frames and lands (TeleportLocomotionClass::Land_Chrono_Warp). A vehicle set down on water it
/// cannot cross sinks there, and anything else set down where it cannot move is destroyed.
/// </summary>
static void Chrono_Shift(FootClass * foot, Coord dest, DynamicVectorClass<FootClass *> const & moving)
{
	Cell cell = dest.As_Cell();
	if (!Map.In_Radar(cell)) {
		Chrono_Kill(foot);
		return;
	}

	DynamicVectorClass<TechnoClass *> crushed;
	bool blocked = false;
	CellClass * cellptr = &Map[cell];
	for (ObjectClass * object = cellptr->Cell_Occupier(cellptr->IsUnderBridge); object != NULL; object = object->Next) {
		if (object == foot || (object->Is_Foot() && moving.ID((FootClass *)object) != -1)) {
			continue;
		}
		if (object->Is_Techno() && ((TechnoClass *)object)->Is_Iron_Curtained()) {
			Chrono_Kill(foot);
			return;
		}
		if (object->RTTI == RTTI_BUILDING || object->RTTI == RTTI_TERRAIN) {
			blocked = true;
		} else if (!object->Is_Foot()) {
			continue;
		} else if (foot->RTTI != RTTI_INFANTRY || object->RTTI != RTTI_INFANTRY || (object->PositionCoord.X == dest.X && object->PositionCoord.Y == dest.Y)) {
			crushed.Add((TechnoClass *)object);
		}
	}

	if (blocked) {
		Cell const nearby = Map.Nearby_Location(cell, foot->TClass->Speed, -1, foot->TClass->MZone);
		if (nearby == CELL_NONE) {
			Chrono_Kill(foot);
			return;
		}
		dest.X += (nearby.X - cell.X) * CELL_LEPTON_W;
		dest.Y += (nearby.Y - cell.Y) * CELL_LEPTON_H;
	} else {
		// A unit that may not be crushed, or a vehicle under infantry when ChronoInfantryCrush is off, destroys the arriving unit instead.
		for (int index = 0; index < crushed.Count(); index++) {
			TechnoClass const * victim = crushed[index];
			if (!victim->TClass->IsChronoshiftCrushable || (foot->RTTI == RTTI_INFANTRY && victim->RTTI != RTTI_INFANTRY && !Rule->IsChronoInfantryCrush)) {
				Chrono_Kill(foot);
				return;
			}
		}
		for (int index = 0; index < crushed.Count(); index++) {
			Chrono_Kill(crushed[index]);
		}
	}

	TechnoTypeClass const * type = foot->TClass;
	VocType const out_sound = type->ChronoOutSound != VOC_NONE ? type->ChronoOutSound : Rule->ChronoOutSound;
	Sound_Effect(out_sound, foot->Center_Coord());

	// The unit keeps its place and its team while the warp runs, and lands through its teleport locomotor.
	foot->Begin_Chrono_Warp(dest);
}


namespace {

enum class ChronoKind {
	Skip,
	Structure,
	Vehicle
};

struct ChronoStructure {
	BuildingClass * Building;
	ChronoKind Kind;
};


// How the chronosphere treats a structure inside its area: left alone, shifted as a structure, or shifted like a unit.
ChronoKind Chrono_Kind_Of(SuperWeaponKeys const & keys, BuildingClass const * building, bool water)
{
	BuildingTypeClass const * type = building->Class;
	if (!building->IsActive || building->IsInLimbo || !type->IsChronoshiftAllowed || type->IsWall) {
		return(ChronoKind::Skip);
	}
	if (!keys.ChronoAffectsUnwarpable && !type->IsWarpable) {
		return(ChronoKind::Skip);
	}
	if (building->Is_Iron_Curtained() && !keys.ChronoAffectsIronCurtain) {
		return(ChronoKind::Skip);
	}
	if (keys.ChronoAffectsUndeployable && type->UndeploysInto != NULL) {
		return(ChronoKind::Vehicle);
	}
	bool const as_unit = keys.ChronoReconsiderBuildings && type->IsChronoshiftVehicle;
	unsigned const wanted = keys.Affected_Targets(SuperRules::TARGET_INFANTRY | SuperRules::TARGET_UNITS);
	if (!SuperRules::Target_Passes(wanted, water, as_unit ? SuperRules::Occupant::Unit : SuperRules::Occupant::Building)) {
		return(ChronoKind::Skip);
	}
	return(as_unit ? ChronoKind::Vehicle : ChronoKind::Structure);
}


// Does the whole foundation of the structure fit on the map and on clear ground with its origin on the cell?
bool Chrono_Structure_Fits(BuildingClass * building, Cell const & where)
{
	for (Cell const * offset = building->Class->Occupy_List(); *offset != REFRESH_EOL; offset++) {
		if (!Map.In_Radar(where + *offset)) {
			return(false);
		}
	}
	return(Map.In_Radar(where) && building->Can_Enter_Cell(&Map[where], FACING_NONE) == MOVE_OK);
}


bool Chrono_Set_Down(BuildingClass * building, Cell const & where, bool restore)
{
	if (restore) {
		// The structure stood here a moment ago, so only another structure that landed on its foundation keeps it out.
		for (Cell const * offset = building->Class->Occupy_List(); *offset != REFRESH_EOL; offset++) {
			if (!Map.In_Radar(where + *offset) || Map[where + *offset].Cell_Building() != NULL) {
				return(false);
			}
		}
	} else if (!Chrono_Structure_Fits(building, where)) {
		return(false);
	}
	ScenarioInit++;
	bool const placed = building->Unlimbo(Map[where].Center_Coord(), DIR_N);
	ScenarioInit--;
	return(placed);
}

}


/// <summary>
/// Moves the structures the chronosphere picked by the offset between the picked cell and the target. Every
/// structure leaves the map first, so structures of the group may land where another stood. A structure
/// lands on its shifted foundation. One shifted like a unit takes the nearest free foundation within six
/// cells of it. A structure that fits nowhere returns to its place and is destroyed when
/// Chronosphere.BlowUnplaceable is on.
/// </summary>
static void Chrono_Shift_Buildings(std::vector<ChronoStructure> const & structures, Cell const & offset, SuperWeaponKeys const & keys, HouseClass * house)
{
	struct Mover {
		BuildingClass * Building;
		ChronoKind Kind;
		Cell Origin;
	};
	std::vector<Mover> movers;
	for (size_t index = 0; index < structures.size(); index++) {
		BuildingClass * building = structures[index].Building;
		if (!building->IsActive || building->IsInLimbo) {
			continue;
		}
		if (keys.ChronoKillCargo) {
			building->Kill_Cargo(NULL);
		}
		Cell const origin = building->PositionCell;
		VocType const out_sound = building->Class->ChronoOutSound != VOC_NONE ? building->Class->ChronoOutSound : Rule->ChronoOutSound;
		Sound_Effect(out_sound, building->Center_Coord());
		building->Limbo();
		if (building->IsInLimbo) {
			movers.push_back({building, structures[index].Kind, origin});
		}
	}

	for (size_t index = 0; index < movers.size(); index++) {
		BuildingClass * building = movers[index].Building;
		Cell const wanted = movers[index].Origin + offset;
		bool placed = Chrono_Set_Down(building, wanted, false);
		if (!placed && movers[index].Kind == ChronoKind::Vehicle) {
			for (int ring = 1; ring <= 6 && !placed; ring++) {
				for (int dy = -ring; dy <= ring && !placed; dy++) {
					for (int dx = -ring; dx <= ring && !placed; dx++) {
						if (std::max(std::abs(dx), std::abs(dy)) == ring) {
							placed = Chrono_Set_Down(building, wanted + Cell(dx, dy), false);
						}
					}
				}
			}
		}

		if (placed) {
			VocType const in_sound = building->Class->ChronoInSound != VOC_NONE ? building->Class->ChronoInSound : Rule->ChronoInSound;
			Sound_Effect(in_sound, building->Center_Coord());
			DebugString("SUPER chronoshift %s from %d,%d to %d,%d\n", building->Class->Name(), (int)movers[index].Origin.X, (int)movers[index].Origin.Y, (int)building->PositionCell.X, (int)building->PositionCell.Y);
		} else if (Chrono_Set_Down(building, movers[index].Origin, true)) {
			if (keys.ChronoBlowUnplaceable) {
				Chrono_Kill(building, house);
			}
			DebugString("SUPER chronoshift %s at %d,%d has no room at the target\n", building->Class->Name(), (int)movers[index].Origin.X, (int)movers[index].Origin.Y);
		} else {
			DebugString("SUPER chronoshift %s at %d,%d cannot return and is removed\n", building->Class->Name(), (int)movers[index].Origin.X, (int)movers[index].Origin.Y);
			building->Delete_Me();
		}
	}
	house->IsRecalcNeeded = true;
}


/// <summary>
/// Warps the units the chronosphere picked to the cell, as the ChronoWarp case of
/// SuperClass::Launch (0x6CC390) does. ChronoBlast plays over the picked cell and
/// ChronoBlastDest over the target. A parasite inside a unit on those cells dies. Every unit on the ground on the picked cell and the eight
/// around it moves to the same place relative to the target; on a bridge cell only the units
/// on the bridge go. Organic units that are not Teleporters are killed instead. Units under
/// the Iron Curtain, units already being chronoshifted or held by a temporal beam, and vehicles
/// standing in a war factory stay behind.
/// The keys of the chronosphere weapon that picked the cells replace this: SW.Range sizes the area,
/// SW.AffectsHouse and SW.AffectsTarget choose the units, Chronosphere.BlastSrc and Chronosphere.BlastDest
/// the animations, and the other Chronosphere.* keys who is killed, skipped or emptied of cargo.
/// </summary>
void SuperClass::Chrono_Warp(Cell const & cell) const
{
	SuperClass const * sphere = Pre_Dependent();
	SuperWeaponKeys const & keys = (sphere != NULL ? sphere->Class : Class)->Keys;
	Coord const from = Map[ChronoSource].As_Coord();
	Coord const to = Map[cell].As_Coord();
	AnimTypeClass const * blast_dest = keys.ChronoBlastDest != NULL ? keys.ChronoBlastDest : Rule->ChronoBlastDest;
	AnimTypeClass const * blast_src = keys.ChronoBlastSrc != NULL ? keys.ChronoBlastSrc : Rule->ChronoBlast;
	if (blast_dest != NULL) {
		new AnimClass(blast_dest, Coord(to.X, to.Y, to.Z + 5));
	}
	if (blast_src != NULL) {
		new AnimClass(blast_src, Coord(from.X, from.Y, from.Z + 5));
	}

	SuperRules::Area const area = keys.Target_Area_Or_Block(3, 3);
	unsigned const wanted = keys.Affected_Targets(SuperRules::TARGET_INFANTRY | SuperRules::TARGET_UNITS);
	int const reach = SuperRules::Area_Reach(area, 512);
	DynamicVectorClass<FootClass *> killed;
	DynamicVectorClass<FootClass *> moving;
	std::vector<ChronoStructure> structures;
	for (int dy = -reach; dy <= reach; dy++) {
		for (int dx = -reach; dx <= reach; dx++) {
			if (!SuperRules::Area_Contains(area, dx, dy)) {
				continue;
			}
			Cell const where = ChronoSource + Cell(dx, dy);
			if (!Map.In_Radar(where)) {
				continue;
			}
			CellClass & cellptr = Map[where];
			BuildingClass * structure = cellptr.Cell_Building();
			if (structure != NULL && structure->House != NULL && SuperRules::House_Passes(keys.AffectsHouse, structure->House == House, House->Is_Ally(structure->House))) {
				bool listed = false;
				for (size_t known = 0; known < structures.size(); known++) {
					listed = listed || structures[known].Building == structure;
				}
				ChronoKind const kind = listed ? ChronoKind::Skip : Chrono_Kind_Of(keys, structure, cellptr.Land_Type() == LAND_WATER);
				if (kind != ChronoKind::Skip) {
					structures.push_back({structure, kind});
				}
			}
			for (ObjectClass * object = cellptr.Cell_Occupier(cellptr.IsUnderBridge); object != NULL; object = object->Next) {
				if (!object->Is_Foot() || object->In_Air()) {
					continue;
				}
				FootClass * foot = (FootClass *)object;

				HouseClass const * owner = foot->Owner_HouseClass();
				if (owner != NULL && !SuperRules::House_Passes(keys.AffectsHouse, owner == House, House->Is_Ally(owner))) {
					continue;
				}
				SuperRules::Occupant const kind = foot->RTTI == RTTI_INFANTRY ? SuperRules::Occupant::Infantry : SuperRules::Occupant::Unit;
				if (!SuperRules::Target_Passes(wanted, cellptr.Land_Type() == LAND_WATER, kind)) {
					continue;
				}

				// A parasite inside a unit the warp touches is doomed for 500 frames and driven out, so it dies (SuperClass::Launch, 0x6CC7B7).
				if (foot->ParasiteEatingMe != NULL && foot->ParasiteEatingMe->ParasiteImUsing) {
					foot->ParasiteEatingMe->ParasiteImUsing->SuppressedUntil = Frame + 500;
					foot->ParasiteEatingMe->ParasiteImUsing->Exit_Unit();
				}

				BuildingClass const * building = cellptr.Cell_Building();
				if (foot->RTTI == RTTI_UNIT && building != NULL && building->Class->IsWeaponsFactory) {
					continue;
				}
				if (!foot->TClass->IsChronoshiftAllowed) {
					continue;
				}
				if (!keys.ChronoAffectsUnwarpable && !foot->TClass->IsWarpable) {
					continue;
				}
				bool const organic = foot->TClass->IsOrganic && !foot->TClass->IsTeleporter;
				if ((organic && keys.ChronoKillOrganic) || (foot->TClass->IsTeleporter && keys.ChronoKillTeleporters)) {
					killed.Add(foot);
				} else if ((!foot->Is_Iron_Curtained() || keys.ChronoAffectsIronCurtain) && !foot->Is_Chrono_Warping() && !foot->IsBeingWarpedOut) {
					moving.Add(foot);
				}
			}
		}
	}

	// The chronosphere's house gets the credit for the organic units it kills (SuperClass::Launch, 0x6CC8C6).
	for (int index = 0; index < killed.Count(); index++) {
		Chrono_Kill(killed[index], House);
	}
	for (int index = 0; index < moving.Count(); index++) {
		FootClass * foot = moving[index];
		if (foot->IsActive && !foot->IsInLimbo) {
			if (keys.ChronoKillCargo) {
				foot->Kill_Cargo(NULL);
			}
			Coord dest = foot->PositionCoord;
			dest.X += to.X - from.X;
			dest.Y += to.Y - from.Y;
			Chrono_Shift(foot, dest, moving);
		}
	}
	if (!structures.empty()) {
		Chrono_Shift_Buildings(structures, Cell(cell.X - ChronoSource.X, cell.Y - ChronoSource.Y), keys, House);
	}
}


/// <summary>
/// Unleashes the super weapon upon the cell specified.
/// This routine is called once the target has been chosen, either by the player
/// clicking on the map or by the computer deciding where to strike. Each kind of super
/// weapon does its own thing here -- some order a launch site to fire, others create
/// their effect outright, and the hunter seeker simply walks out of the nearest
/// suitable building.
/// </summary>
/// <param name="cell">The cell that was designated as the target.</param>
/// <param name="player">Is this for a player owned super weapon? If so, then the
/// targeting cursor gets cleared.</param>
void SuperClass::Place(Cell const & cell, bool player)
{
	if (IsReady && Class->Type != SUPER_CHRONOSPHERE && !Class->UseChargeDrain) {
		Play_Target_Effects(cell);
		Show_Message(Class->Keys.MessageActivate);
	}

	switch (Class->Type) {
		case SUPER_DROP_PODS:
			Drop_Pods(cell);
			if (player) {
				Map.IsTargettingMode = SUPER_NONE;
			}
			House->IsRecalcNeeded = true;
			break;

		case SUPER_GENERIC_WARHEAD:
			if (IsReady) {
				Generic_Warhead(cell);
				Speak_Activated(NULL);
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_UNIT_DELIVERY:
			if (IsReady) {
				Unit_Delivery(cell);
				Speak_Activated(NULL);
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_SONAR_PULSE:
			if (IsReady) {
				Sonar_Pulse(cell);
				Speak_Activated(NULL);
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_BATTERY:
			Speak_Activated(NULL);
			House->RecalcPower = true;
			if (player) {
				Map.Flag_Strips_To_Redraw();
			}
			break;

		case SUPER_PSYCHIC_REVEAL:
			if (IsReady) {
				// Uncovers the area for the firing house, shroud and fog (the PsychicReveal case of SuperClass::Launch).
				// SW.Range sets the radius; a negative one uncovers the whole map.
				int radius = Rule->PsychicRevealRadius;
				if (Class->Keys.AreaIsSet && !Class->Keys.AreaIsBlock) {
					radius = Class->Keys.AreaRadius < 0.0f ? 400 : (int)Class->Keys.AreaRadius;
				}
				for (int dy = -radius; dy <= radius; dy++) {
					for (int dx = -radius; dx <= radius; dx++) {
						Cell const where = cell + Cell(dx, dy);
						if (Map.In_Radar(where) && Gamemd_Distance(where, cell) <= radius) {
							Map.Map_Cell(where, House);
							Map.Fog_Map_Cell(where, House);
						}
					}
				}
				Sound_Effect(Rule->PsychicRevealActivateSound, Map[cell].Center_Coord());
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_PSYCHIC_DOMINATOR:
			if (IsReady) {
				Submit_Radar_Event(RADAREVENT_SUPERWEAPON, cell);
				PsychicDominatorClass::Start(cell, House, Class);
				Sound_Effect(Rule->PsychicDominatorActivateSound, Map[cell].Center_Coord());
				Speak_Activated("EVA_PsychicDominatorActivated");
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_GENETIC_CONVERTER:
			if (IsReady) {
				Genetic_Mutator(cell);
				Speak_Activated("EVA_GeneticMutatorActivated");
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_FORCE_SHIELD:
			if (IsReady) {
				Force_Shield(cell);
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_LIGHTNING_STORM:
			if (IsReady) {
				int const duration = Class->Keys.LightningDuration != SuperRules::UNSET_INT ? Class->Keys.LightningDuration : Rule->LightningStormDuration;
				LightningStormClass::Start(duration, SuperRules::Effective_Deferment(Class->Keys.Deferment, Rule->LightningStormDeferment), cell, House, Class);
				Speak_Activated("EVA_LightningStormCreated");
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_CHRONOSPHERE:
			if (IsReady) {
				ChronoSource = cell;
				Stop_Pre_Click_Anim();
				AnimTypeClass const * placement = Class->Keys.Animation != NULL ? Class->Keys.Animation : Rule->ChronoPlacement;
				if (placement != NULL) {
					Coord const coord = Map[cell].As_Coord();
					PreClickAnim = new AnimClass(placement, Coord(coord.X, coord.Y, coord.Z + (Class->Keys.Animation != NULL ? Class->Keys.AnimationHeight : 5)));
				}
				if (player) {
					Map.IsTargettingMode = Follow_Up();
				} else if (PreClickAnim != NULL) {
					PreClickAnim->Make_Invisible();
				}
			}
			break;

		case SUPER_PARA_DROP:
		case SUPER_AMER_PARA_DROP:
		case SUPER_SPY_PLANE:
			if (IsReady) {
				Paradrop(cell);
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_CHRONO_WARP:
			Submit_Radar_Event(RADAREVENT_SUPERWEAPON, ChronoSource);
			Submit_Radar_Event(RADAREVENT_SUPERWEAPON, cell);
			Chrono_Warp(cell);
			Speak_Activated("EVA_ChronosphereActivated");
			if (player) {
				Map.IsTargettingMode = SUPER_NONE;
			}
			House->IsRecalcNeeded = true;
			break;

		case SUPER_IRON_CURTAIN:
			if (IsReady) {
				Iron_Curtain(cell);
				Speak_Activated("EVA_IronCurtainActivated");
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_ION_CANNON:
			if (IsReady) {
				Coord coord = cell.As_Coord(LEVEL_LEPTON_H * Map[cell].Height);
				if (coord != COORD_NONE) {
					if (player) {
						Map.IsTargettingMode = SUPER_NONE;
					}
					House->IsRecalcNeeded = true;
					new IonBlastClass(coord);
				}
			}
			break;

		case SUPER_EM_PULSE:
			if (IsReady && (!IsOneTime || !IsPresent)) {
				EM_Pulse_Launch(cell);
				if (player) {
					Map.IsTargettingMode = SUPER_NONE;
				}
				House->IsRecalcNeeded = true;
			}
			break;

		case SUPER_FIRESTORM:
			House->Activate_Firestorm();
			if (player) {
				House->IsRecalcNeeded = true;
				Map.Flag_Strips_To_Redraw();
			}
			break;

		case SUPER_MULTI_MISSILE:
		case SUPER_CHEM_MISSILE:
			if (IsReady) {
				// A one-time weapon, and a MultiMissile with Nuke.SiloLaunch=no, flies in from the map edge rather than from a silo.
				if ((IsOneTime && IsPresent) || (Class->Type == SUPER_MULTI_MISSILE && !Class->Keys.NukeSiloLaunch)) {
					Cell target = cell;
					Coord start = target.As_Coord();
					start.Z = Map.Get_Height_GL(start);
					if (Map[target].IsUnderBridge) {
						start.Z += BRIDGE_LEPTON_HEIGHT;
					}

					Cell closest = Map.Closest_Edge_Cell(start);

					// The stock launcher weapons are not in every rules file; the payload weapon stands in when they are missing.
					auto weapon_named = [](char const * name) -> WeaponTypeClass const * {
						WeaponType const found = WeaponTypeClass::From_Name(name);
						return(found >= 0 && found < Weapons.Count() ? Weapons[found] : NULL);
					};
					WeaponTypeClass const * wtype;
					if (Class->Type == SUPER_CHEM_MISSILE) {
						wtype = weapon_named("ChemLauncher");
					} else {
						wtype = Class->Keys.NukePayload;
						if (wtype == NULL) wtype = weapon_named("MultiLauncher");
						if (wtype == NULL) wtype = weapon_named("NukePayload");
					}

					// SW.Damage and SW.Warhead replace the payload's own when the weapon sets them.
					BulletClass * bullet = NULL;
					if (wtype != NULL && wtype->Bullet != NULL) {
						int const strike_damage = Class->Keys.Damage >= 0 ? Class->Keys.Damage : wtype->Attack;
						WarheadTypeClass const * strike_warhead = Class->Keys.Warhead != NULL ? Class->Keys.Warhead : wtype->WarheadPtr;
						bullet = Create_Bullet(wtype->Bullet, &Map[start], NULL, strike_damage, strike_warhead, wtype->MaxSpeed, false);
					}

					if (bullet) {
						bullet->NukeSuper = Class->HeapID;
						TVelocity3D<double> velocity = TVelocity3D<double>(DIR_E, DIR_N, 100);
						bullet->Unlimbo(closest, velocity);
					}
					if (Class->Type == SUPER_MULTI_MISSILE) {
						Speak_Activated("EVA_NuclearMissileLaunched");
					}
					if (player) {
						Map.IsTargettingMode = SUPER_NONE;
					}
					House->IsRecalcNeeded = true;
				} else {
					for (int index = 0; index < BuildingTypes.Count(); index++) {
						BuildingTypeClass const * btype = BuildingTypes[index];
						if (btype->IsNukeSilo && btype->Grants_Super_Weapon(Class->HeapID)) {

							/*
							**	Search for a suitable launch site for this missile.
							*/
							BuildingClass * launchsite = House->Find_Building(StructType(index));

							/*
							**	If a launch site was found, then proceed with the normal launch
							**	sequence.
							*/
							if (launchsite) {
								launchsite->Assign_Mission(MISSION_MISSILE);
								launchsite->Commence();
								House->NukeDest = cell;
								launchsite->LastSuperWeaponIndex = Class->HeapID;
								if (Class->Type == SUPER_MULTI_MISSILE) {
									Speak_Activated("EVA_NuclearMissileLaunched");
								}
							}

							break;
						}
					}
					if (player) {
						Map.IsTargettingMode = SUPER_NONE;
					}
					House->IsRecalcNeeded = true;
				}
			}
			break;

		case SUPER_HUNTER_SEEKER:
			Hunter_Seeker_Launch();
		break;
	}
}


/// <summary>
/// Shuts the firestorm defense back down.
/// This routine is called when the firestorm wall is toggled off or its charge runs
/// out. Any other kind of super weapon ignores the request.
/// </summary>
/// <param name="player">Is this for a player owned super weapon? If so, then the sidebar
/// will want redrawing.</param>
void SuperClass::Deactivate_Firestorm(int, bool player) const
{
	if (Class->Type == SUPER_BATTERY) {
		House->RecalcPower = true;
		if (player) {
			Map.Flag_Strips_To_Redraw();
		}
	}
	if (Class->Type == SUPER_FIRESTORM) {
		House->Deactivate_Firestorm();
		if (player) {
			House->IsRecalcNeeded = true;
			Map.Flag_Strips_To_Redraw();
		}
	}
}


/// <summary>
/// Is the super weapon building up toward its next use?
/// This routine is used by the sidebar to decide whether the charge-up clock should be
/// drawn over the cameo. A charge-drain weapon is always considered to be charging,
/// since it never simply sits at the ready.
/// </summary>
/// <returns>bool; Is the super weapon charging?</returns>
bool SuperClass::Is_Charging(void) const
{
	if (Class->UseChargeDrain) {
		return(true);
	}
	return(IsReady == false);
}


ClassID SuperClass::Class_ID(void) const
{
	return(ClassID_SuperWeaponClass);
}


/// <summary>
/// Tells whether the sidebar tab that holds this super weapon should flash.
/// A suspended weapon never flashes. A charge drain weapon flashes unless it is charging; any
/// other weapon flashes only while it is ready. FlashSidebarTabFrames of -1 keeps it flashing,
/// and a positive count keeps it flashing only before ReadyFrame plus that count. ReadyFrame is
/// set when a recharge finishes, not when the charge is set or forced.
/// </summary>
bool SuperClass::Should_Flash_Tab(void) const
{
	if (IsSuspended) {
		return(false);
	}

	if (Class->UseChargeDrain) {
		if (ChargeDrainState == CHARGING) {
			return(false);
		}
	} else if (!IsReady) {
		return(false);
	}

	int const frames = Class->FlashSidebarTabFrames;
	if (frames == -1) {
		return(true);
	}
	return(frames != 0 && Frame < ReadyFrame + frames);
}


/// <summary>
/// Lists the members this super weapon carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void SuperClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(Class);
	stream.Serialize(House);
	stream.Serialize(Control);
	stream.Serialize(NeedsBuilding);
	stream.Serialize(IsPresent);
	stream.Serialize(IsOneTime);
	stream.Serialize(IsReady);
	stream.Serialize(IsSuspended);
	stream.Serialize(OldStage);
	stream.Serialize(SpecialSoundTimer);
	stream.Serialize(IsSpecialSoundPending);
	stream.Serialize(SpecialSoundCoord);
	stream.Serialize(ChronoSource);
	stream.Serialize(PreClickAnim);
	stream.Serialize(ChargeDrainState);
	stream.Serialize(CustomRechargeTime);
	stream.Serialize(Pending);
	stream.Serialize(ShotsFired);
	stream.Serialize(WasEnabled);
	stream.Serialize(IsVirtualCharging);
	stream.Serialize(ReadyFrame);
}


/// <summary>
/// Removes any reference to the object specified.
/// This routine is called when an object is about to be destroyed, so that the super
/// weapon does not keep pointing at the type or the house that is going away.
/// </summary>
/// <param name="target">The object that is about to disappear.</param>
void SuperClass::Detach(AbstractClass const * target, bool all)
{
	if (target == House) {
		House = NULL;
	}
	if (target == Class) {
		Class = NULL;
	}
	if (target == PreClickAnim) {
		PreClickAnim = NULL;
	}
}


/// <summary>
/// Submits the super weapon's state to the game CRC.
/// This routine is used by the multiplayer synchronization check to prove that every
/// machine agrees about the charge state of this super weapon.
/// </summary>
void SuperClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(IsPresent);
	crc(IsOneTime);
	crc(IsReady);
	crc(OldStage);
	crc(ChargeDrainState);
	crc(NeedsBuilding);
	crc(CustomRechargeTime);
	crc((int)Pending.size());
	crc(ShotsFired);
	crc(WasEnabled);
	crc(IsVirtualCharging);
}


/// <summary>
/// Delivers a squad of elite infantry by drop pod.
/// This routine is used by the drop pod super weapon to land a handful of veteran
/// soldiers on the map. Each pod aims for a cell adjacent to the one before it, so the
/// squad arrives as a cluster rather than stacked on one spot, and any soldier that
/// cannot find room to land is quietly thrown away.
/// </summary>
/// <param name="cell">The cell to begin dropping the squad around.</param>
void SuperClass::Drop_Pods(Cell const & cell) const
{
	Cell working_cell = cell;
	SuperWeaponKeys const & keys = Class->Keys;
	int const minimum = keys.DropPodMinimum >= 0 ? keys.DropPodMinimum : Rule->DropPodInfantryMinimum;
	int const maximum = keys.DropPodMaximum >= 0 ? std::max(keys.DropPodMaximum, minimum) : Rule->DropPodInfantryMaximum;
	int count = Random_Pick(minimum, maximum);
	InfantryType i1 = InfantryTypeClass::From_Name("E1");
	InfantryTypeClass const * inftype = InfantryTypes[i1];
	InfantryType i2 = InfantryTypeClass::From_Name("E2");
	InfantryTypeClass const * altinftype = InfantryTypes[i2];

	int toplace = count;
	int attempts = 3 * count;

	while (toplace && attempts--) {
		InfantryTypeClass const * chosen = (Random_Double(0.0, 1.0) < 0.5) ? inftype : altinftype;
		if (keys.DropPodTypes.Count() > 0) {
			// DropPod.Types names the infantry; a type listed twice is twice as likely.
			TechnoTypeClass const * listed = keys.DropPodTypes[Random_Pick(0, keys.DropPodTypes.Count() - 1)];
			if (listed != NULL && listed->Fetch_RTTI() == RTTI_INFANTRYTYPE) {
				chosen = static_cast<InfantryTypeClass const *>(listed);
			}
		}
		InfantryClass * inf = (InfantryClass *)(chosen->Create_One_Of(House));

		Cell nearby = Map.Nearby_Location(working_cell, SPEED_FOOT, -1, MZONE_INFANTRY, false, Point2D(1,1), false, false, false, false);
		if (inf->Can_Enter_Cell(&Map[nearby]) == MOVE_OK) {
			// DropPod.Veterancy of 2 or more makes elite pod troops, 1 veteran ones.
			if (keys.DropPodVeterancy >= 2.0f) {
				inf->Veterancy.Set_Elite(true);
			} else if (keys.DropPodVeterancy >= 1.0f) {
				inf->Veterancy.Set_Veteran(true);
			}
			inf->Link_DropPod();
			inf->PositionCoord = nearby;
			inf->Assign_Destination(&Map[nearby]);
			inf->Locomotion->Move_To(Coord(nearby));
			if (!inf->IsInLimbo) {
				inf->Look();
				inf->Assign_Mission(MISSION_GUARD_AREA);
				inf->Commence();
				toplace--;
			}

			FacingType start_dir = Random_Pick(FACING_N, FACING_NW);

			for (int offset = 0; offset < FACING_COUNT; offset++) {
				FacingType dir = (FacingType)((start_dir + offset) % FACING_COUNT);
				if (inf->Can_Enter_Cell(&Map[Adjacent_Cell(nearby, dir)]) == MOVE_OK) {
					working_cell = Adjacent_Cell(nearby, dir);
					break;
				}
			}

			if (inf->IsInLimbo) {
				inf->Delete_Me();
			}
		} else {
			inf->Delete_Me();
		}
	}
}
