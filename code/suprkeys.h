/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>
#include <vector>

#include "swrules.h"
#include "typelist.h"
#include "voc.hh"

class AircraftTypeClass;
class AnimTypeClass;
class BuildingTypeClass;
class CCINIClass;
class SaveStreamClass;
class TechnoTypeClass;
class UnitTypeClass;
class WarheadTypeClass;
class WeaponTypeClass;
class CRCEngine;


/*
 * Conversions from a key's text to the value a superweapon keeps. Each returns the value to keep: the old one
 * when the text is empty, so a key a section leaves out changes nothing. A list replaces the old one whenever
 * the text is not empty, and "none" empties it.
 */
namespace SuperKeyParse
{
	std::string Text_Or(std::string const & value, std::string const & old);
	void Building_List(std::string const & value, TypeList<BuildingTypeClass const *> & list);
	void Techno_List(std::string const & value, TypeList<TechnoTypeClass const *> & list);
	WarheadTypeClass const * Warhead_Named(std::string const & value, WarheadTypeClass const * old);
	AnimTypeClass const * Anim_Named(std::string const & value, AnimTypeClass const * old);
	UnitTypeClass const * Unit_Named(std::string const & value, UnitTypeClass const * old);
	AircraftTypeClass const * Aircraft_Named(std::string const & value, AircraftTypeClass const * old);
	WeaponTypeClass const * Weapon_Named(std::string const & value, WeaponTypeClass const * old);
	int Deliver_Owner(std::string const & value, int old);
}


/*
 * The superweapon keys that Ares adds (SW.*, Money.Amount, EVA.*, Message.*, Text.* and the
 * per-behavior keys such as Deliver.Types), as one superweapon type reads them from its rules
 * section. A weapon keeps the defaults below for a key its section leaves out.
 */
struct SuperWeaponKeys
{
	SuperWeaponKeys(void);

	void Set_Area(SuperRules::Area const & area);
	void Set_AI_Targeting(std::string const & value);
	void Set_AI_Requires_House(std::string const & value);
	void Set_Pulse_Ball(std::string const & value);
	void Read_Para_Owners(CCINIClass const & ini, char const * section);
	void Serialize(SaveStreamClass & stream);
	void Compute_CRC(CRCEngine & crc) const;

	/*
	 * Cost, timing and limits. A negative Money.Amount is taken from the house when the weapon
	 * fires; a positive one is paid to it. Deferment is -1 until the section sets it.
	 */
	int MoneyAmount;
	int Deferment;
	int Shots;
	int MaxCount;
	int Group;
	bool InitialReady;
	bool AllowPlayer;
	bool AllowAI;
	bool Unstoppable;
	float ChargeToDrainRatio;

	// Which houses and which buildings make the weapon available.
	TypeList<BuildingTypeClass const *> AuxBuildings;
	TypeList<BuildingTypeClass const *> NegBuildings;
	TypeList<int> RequiredHouses;
	TypeList<int> ForbiddenHouses;

	/*
	 * Where the weapon may be aimed and what it affects. RequiresTarget is zero until the
	 * section sets it, which allows every cell; the Affects values hold Parse_Houses and
	 * Parse_Targets flags.
	 */
	unsigned AffectsHouse;
	bool AffectsHouseSet;
	unsigned AffectsTarget;
	unsigned RequiresTarget;
	unsigned RequiresHouse;
	bool FireIntoShroud;
	float RangeMinimum;
	float RangeMaximum;
	bool AreaIsSet;
	bool AreaIsBlock;
	float AreaRadius;
	int AreaWidth;
	int AreaHeight;
	TypeList<TechnoTypeClass const *> Designators;
	TypeList<TechnoTypeClass const *> Inhibitors;
	bool AnyDesignator;
	bool AnyInhibitor;

	// How the computer and automatic firing choose a target.
	int AITargeting;
	unsigned AIRequiresTarget;
	unsigned AIRequiresHouse;
	unsigned AIConstraints;
	bool UseAITargeting;
	bool AutoFire;
	bool ManualFire;
	bool ShowCameo;

	// The effect: a warhead and damage, an animation and sounds at the target.
	WarheadTypeClass const * Warhead;
	int Damage;
	AnimTypeClass const * Animation;
	int AnimationHeight;
	VocType Sound;
	VocType ActivationSound;
	bool CreateRadarEvent;

	/*
	 * Announcements. A string left empty keeps the behavior's own line; "none" turns the line
	 * off. Messages are string table labels.
	 */
	std::string EvaReady;
	std::string EvaDetected;
	std::string EvaActivated;
	std::string EvaImpatient;
	std::string EvaSelectTarget;
	std::string EvaInsufficientFunds;
	std::string MessageDetected;
	std::string MessageReady;
	std::string MessageLaunch;
	std::string MessageActivate;
	std::string MessageAbort;
	std::string MessageInsufficientFunds;
	std::string MessageCannotFire;
	bool MessageFirerColor;
	std::string TextHold;
	std::string TextReady;
	std::string TextCharging;
	std::string TextActive;
	std::string TextPreparing;

	// Names of the mouse cursors for a place the weapon may fire at and one it may not.
	std::string Cursor;
	std::string NoCursor;

	// Screen lighting while the weapon acts; -1 keeps the scenario value.
	bool LightEnabled;
	int LightAmbient;
	int LightRed;
	int LightGreen;
	int LightBlue;

	// UnitDelivery.
	TypeList<TechnoTypeClass const *> DeliverTypes;
	int DeliverOwnerMode;
	bool DeliverBaseNormal;
	bool DeliverBuildups;

	// DropPod. Minimum and Maximum are -1 until the section sets them.
	TypeList<TechnoTypeClass const *> DropPodTypes;
	int DropPodMinimum;
	int DropPodMaximum;
	float DropPodVeterancy;

	// HunterSeeker.
	TypeList<BuildingTypeClass const *> HunterSeekerBuildings;
	UnitTypeClass const * HunterSeekerType;
	bool HunterSeekerRandomOnly;

	// EMPulse.
	TypeList<BuildingTypeClass const *> EMPulseCannons;
	bool EMPulseTargetSelf;
	bool EMPulseLinked;
	AnimTypeClass const * EMPulsePulseBall;
	bool EMPulseNoPulseBall;
	int EMPulsePulseDelay;

	// MultiMissile: the payload weapon, and whether a building with NukeSilo=yes launches it.
	WeaponTypeClass const * NukePayload;
	bool NukeSiloLaunch;

	// SonarPulse.
	int SonarPulseDelay;

	// Battery.
	int BatteryPower;
	TypeList<BuildingTypeClass const *> BatteryOverpower;
	TypeList<BuildingTypeClass const *> BatteryKeepOnline;

	// SpyPlane.
	AircraftTypeClass const * SpyPlaneType;
	int SpyPlaneCount;

	/*
	 * ParaDrop and AmerParaDrop. A group holds the types, counts and plane for one country or
	 * side; Default holds the plain ParaDrop.* keys.
	 */
	struct ParaGroup
	{
		void Serialize(SaveStreamClass & stream);

		std::string Owner;
		TypeList<TechnoTypeClass const *> Types;
		TypeList<int> Num;
		AircraftTypeClass const * Aircraft = nullptr;
		bool IsSet = false;
	};
	ParaGroup ParaDefault;
	std::vector<ParaGroup> ParaByOwner;
	int ParaDropCount;

	// May this building fire the EMPulse weapon? Every cannon may when the weapon lists none.
	bool Allows_Cannon(BuildingTypeClass const * type) const;

	// Does the weapon name no restriction on where it may be aimed?
	bool Has_Target_Rules(void) const;

	SuperRules::Area Target_Area(void) const;
};
