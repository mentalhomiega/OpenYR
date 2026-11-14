/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "suprkeys.h"

#include "_warhead.h"
#include "_weapon.h"
#include "airctype.h"
#include "animtype.h"
#include "builtype.h"
#include "ccini.h"
#include "crc.h"
#include "dbgprint.h"
#include "globals.h"
#include "houstype.h"
#include "infatype.h"
#include "mission.hh"
#include "savestream.h"
#include "side.h"
#include "techtype.h"
#include "unittype.h"
#include "warhead.h"
#include "weapon.h"

#include <cstring>

using namespace SuperRules;


namespace {

// A value that no flag combination reaches, which marks a mask the section never set.
unsigned const UNSET_MASK = 0xFFFFFFFFu;


template<class T>
T const * Find_Named(DynamicVectorClass<T *> const & vector, char const * name)
{
	for (int index = 0; index < vector.Count(); index++) {
		if (vector[index] != NULL && strcmpi(vector[index]->Name(), name) == 0) {
			return(vector[index]);
		}
	}
	return(NULL);
}


}


SuperWeaponKeys::SuperWeaponKeys(void) :
	MoneyAmount(0),
	Deferment(-1),
	Shots(-1),
	MaxCount(1),
	Group(0),
	InitialReady(false),
	VirtualCharge(false),
	AlwaysGranted(false),
	AllowPlayer(true),
	AllowAI(true),
	Unstoppable(false),
	ChargeToDrainRatio(0.0f),
	AffectsHouse(HOUSE_ALL),
	AffectsHouseSet(false),
	AffectsTarget(0),
	AffectsTargetSet(false),
	RequiresTarget(0),
	RequiresHouse(0),
	FireIntoShroud(true),
	RangeMinimum(-1.0f),
	RangeMaximum(-1.0f),
	AreaIsSet(false),
	AreaIsBlock(false),
	AreaRadius(0.0f),
	AreaWidth(0),
	AreaHeight(0),
	AnyDesignator(false),
	AnyInhibitor(false),
	AITargeting(int(AITarget::Default)),
	AIRequiresTarget(UNSET_MASK),
	AIRequiresHouse(UNSET_MASK),
	AIConstraints(0),
	AIPreference(int(SuperRules::AIPreference::Default)),
	UseAITargeting(false),
	AutoFire(false),
	ManualFire(true),
	ShowCameo(true),
	TimerVisibility(SuperRules::HOUSE_ALL),
	Warhead(NULL),
	Damage(-1),
	Animation(NULL),
	AnimationHeight(5),
	Sound(VOC_NONE),
	ActivationSound(VOC_NONE),
	CreateRadarEvent(false),
	MessageFirerColor(false),
	LightEnabled(true),
	LightAmbient(-1),
	LightRed(-1),
	LightGreen(-1),
	LightBlue(-1),
	DeliverOwnerMode(int(DeliverOwner::Invoker)),
	DeliverBaseNormal(true),
	DeliverBuildups(false),
	DropPodMinimum(-1),
	DropPodMaximum(-1),
	DropPodVeterancy(2.0f),
	HunterSeekerType(NULL),
	HunterSeekerRandomOnly(false),
	EMPulseTargetSelf(false),
	EMPulseLinked(false),
	EMPulsePulseBall(NULL),
	EMPulseNoPulseBall(false),
	EMPulsePulseDelay(32),
	ProtectDuration(-1),
	ProtectPowerOutage(-1),
	ProtectFadeSoundTime(-1),
	NukePayload(NULL),
	NukeSiloLaunch(true),
	LightningDuration(UNSET_INT),
	LightningRadarOutage(UNSET_INT),
	LightningRadarOutageAffects(HOUSE_ENEMIES),
	LightningHitDelay(UNSET_INT),
	LightningScatterDelay(UNSET_INT),
	LightningScatterCount(1),
	LightningSeparation(UNSET_INT),
	LightningPrintText(-1),
	LightningDebrisMin(2),
	LightningDebrisMax(4),
	LightningCloudHeight(-1),
	LightningBoltExplosion(NULL),
	LightningSoundsSet(false),
	LightningCloudsSet(false),
	LightningBoltsSet(false),
	LightningDebrisSet(false),
	LightningIgnoreRod(false),
	DominatorFirstAnim(NULL),
	DominatorSecondAnim(NULL),
	DominatorControlAnim(NULL),
	DominatorFirstAnimHeight(0),
	DominatorSecondAnimHeight(0),
	DominatorFireAtPercentage(UNSET_INT),
	DominatorCapture(true),
	DominatorRipple(true),
	DominatorCaptureMindControlled(true),
	DominatorCapturePermaMindControlled(true),
	DominatorCaptureImmuneToPsionics(false),
	DominatorPermanentCapture(true),
	ChronoBlastSrc(NULL),
	ChronoBlastDest(NULL),
	ChronoKillOrganic(true),
	ChronoKillTeleporters(false),
	ChronoAffectsIronCurtain(false),
	ChronoAffectsUnwarpable(true),
	ChronoKillCargo(false),
	ChronoReconsiderBuildings(true),
	ChronoAffectsUndeployable(false),
	ChronoBlowUnplaceable(true),
	MutateExplosion(-1),
	MutateIgnoreCyborg(false),
	MutateIgnoreNotHuman(false),
	MutateKillNatural(true),
	SonarPulseDelay(60),
	BatteryPower(0),
	SpyPlaneType(NULL),
	SpyPlaneCount(1),
	SpyPlaneMission(MISSION_SPYPLANE_APPROACH),
	ParaDropCount(1)
{
}


namespace SuperKeyParse {

std::string Text_Or(std::string const & value, std::string const & old)
{
	std::string_view const trimmed = Trim(value);
	return(trimmed.empty() ? old : std::string(trimmed));
}


void Building_List(std::string const & value, TypeList<BuildingTypeClass const *> & list)
{
	if (Trim(value).empty()) {
		return;
	}
	list.Clear();
	for (std::string const & name : Split_List(value)) {
		BuildingTypeClass const * type = Find_Named(BuildingTypes, name.c_str());
		if (type != NULL) {
			list.Add(type);
		} else if (!Same(name, "none")) {
			DebugString("A superweapon key names no such building type: %s; skipped.\n", name.c_str());
		}
	}
}


void Techno_List(std::string const & value, TypeList<TechnoTypeClass const *> & list)
{
	if (Trim(value).empty()) {
		return;
	}
	list.Clear();
	for (std::string const & name : Split_List(value)) {
		TechnoTypeClass const * type = Find_Named(TechnoTypes, name.c_str());
		if (type != NULL) {
			list.Add(type);
		} else if (!Same(name, "none")) {
			DebugString("A superweapon key names no such object type: %s; skipped.\n", name.c_str());
		}
	}
}


WarheadTypeClass const * Warhead_Named(std::string const & value, WarheadTypeClass const * old)
{
	std::string const name(Trim(value));
	return(name.empty() ? old : Find_Named(Warheads, name.c_str()));
}


AnimTypeClass const * Anim_Named(std::string const & value, AnimTypeClass const * old)
{
	std::string const name(Trim(value));
	return(name.empty() ? old : Find_Named(AnimTypes, name.c_str()));
}


UnitTypeClass const * Unit_Named(std::string const & value, UnitTypeClass const * old)
{
	std::string const name(Trim(value));
	return(name.empty() ? old : Find_Named(UnitTypes, name.c_str()));
}


AircraftTypeClass const * Aircraft_Named(std::string const & value, AircraftTypeClass const * old)
{
	std::string const name(Trim(value));
	return(name.empty() ? old : Find_Named(AircraftTypes, name.c_str()));
}


WeaponTypeClass const * Weapon_Named(std::string const & value, WeaponTypeClass const * old)
{
	std::string const name(Trim(value));
	return(name.empty() ? old : Find_Named(Weapons, name.c_str()));
}


void Anim_List(std::string const & value, TypeList<AnimTypeClass const *> & list, bool & is_set)
{
	if (Trim(value).empty()) {
		return;
	}
	list.Clear();
	is_set = true;
	for (std::string const & name : Split_List(value)) {
		AnimTypeClass const * type = Find_Named(AnimTypes, name.c_str());
		if (type != NULL) {
			list.Add(type);
		} else if (!Same(name, "none")) {
			DebugString("A superweapon key names no such animation: %s; skipped.\n", name.c_str());
		}
	}
}


int Deliver_Owner(std::string const & value, int old)
{
	return(Trim(value).empty() ? old : (int)Parse_Deliver_Owner(value));
}

}


void SuperWeaponKeys::Set_Area(SuperRules::Area const & area)
{
	if (area.IsSet) {
		AreaIsSet = true;
		AreaIsBlock = area.IsBlock;
		AreaRadius = area.Radius;
		AreaWidth = area.Width;
		AreaHeight = area.Height;
	}
}


void SuperWeaponKeys::Set_AI_Targeting(std::string const & value)
{
	AITarget const mode = Parse_AI_Target(value);
	if (mode != AITarget::Default) {
		AITargeting = int(mode);
	}
}


void SuperWeaponKeys::Set_AI_Requires_House(std::string const & value)
{
	unsigned flags = 0;
	bool found = false;
	for (std::string const & name : Split_List(value)) {
		if (Same(name, "others")) {
			flags |= HOUSE_ALLIES | HOUSE_ENEMIES;
			found = true;
		} else {
			unsigned const one = Parse_Houses(name, UNSET_MASK);
			if (one != UNSET_MASK) {
				flags |= one;
				found = true;
			}
		}
	}
	if (found) {
		AIRequiresHouse = flags;
	}
}


void SuperWeaponKeys::Set_Pulse_Ball(std::string const & value)
{
	std::string const name(Trim(value));
	if (name.empty()) {
		return;
	}
	EMPulseNoPulseBall = Same(name, "none");
	EMPulsePulseBall = EMPulseNoPulseBall ? NULL : Find_Named(AnimTypes, name.c_str());
}


/// <summary>
/// Reads the ParaDrop.<country>.* and ParaDrop.<side>.* keys, whose names depend on the countries and sides
/// the rules declare, into one group for each name that sets any of them.
/// </summary>
void SuperWeaponKeys::Read_Para_Owners(CCINIClass const & ini, char const * section)
{
	auto read_owner = [&](char const * name, int plane_number) {
		std::string prefix = std::string("ParaDrop.") + name + ".";
		if (name[0] == '\0') prefix = "ParaDrop.";
		if (plane_number > 1) prefix += "Plane" + std::to_string(plane_number) + ".";
		else if (name[0] == '\0') return;
		ParaGroup group;
		group.Owner = name;
		group.Plane = plane_number;
		for (ParaGroup const & old : ParaByOwner) {
			if (Same(old.Owner, name) && old.Plane == plane_number) group = old;
		}
		std::string const types = ini.Get_String(section, (prefix + "Types").c_str());
		if (!Trim(types).empty()) {
			SuperKeyParse::Techno_List(types, group.Types);
			group.IsSet = true;
		}
		std::string const nums = prefix + "Num";
		if (ini.Is_Present(section, nums.c_str())) {
			group.Num = ini.Get_IntList(section, nums.c_str(), group.Num);
			group.IsSet = true;
		}
		std::string const plane = ini.Get_String(section, (prefix + "Aircraft").c_str());
		if (!Trim(plane).empty()) {
			group.Aircraft = SuperKeyParse::Aircraft_Named(plane, group.Aircraft);
			group.IsSet = true;
		}
		if (!group.IsSet) return;
		for (ParaGroup & old : ParaByOwner) {
			if (Same(old.Owner, name) && old.Plane == plane_number) { old = group; return; }
		}
		ParaByOwner.push_back(group);
	};
	for (int plane_number = 1; plane_number <= ParaDropCount && plane_number <= MAX_PARA_PLANES; plane_number++) {
		for (int index = 0; index < HouseTypes.Count(); index++) {
			read_owner(HouseTypes[index]->Name(), plane_number);
		}
		for (int index = 0; index < Sides.Count(); index++) {
			read_owner(Sides[index]->Name(), plane_number);
		}
		read_owner("", plane_number);
	}
}


SuperWeaponKeys::ParaGroup const * SuperWeaponKeys::Find_Para(char const * owner, int plane) const
{
	if (owner[0] == '\0' && plane == 1) {
		return ParaDefault.IsSet ? &ParaDefault : nullptr;
	}
	for (ParaGroup const & group : ParaByOwner) {
		if (group.Plane == plane && Same(group.Owner, owner)) return &group;
	}
	return nullptr;
}


bool SuperWeaponKeys::Para_Plan(int plane, char const * country, char const * side, ParaPlan & plan) const
{
	char const * const owners[] = {country, side, ""};
	bool found = false;
	// Plane 1 has only the second three places; a later plane tries its own three first.
	for (int pass = plane > 1 ? 0 : 1; pass < 2; pass++) {
		for (char const * owner : owners) {
			ParaGroup const * group = Find_Para(owner, pass == 0 ? plane : 1);
			if (group == nullptr) continue;
			found = true;
			if (plan.Types == nullptr && (group->Types.Count() > 0 || group->Num.Count() > 0)) {
				plan.Types = &group->Types;
				plan.Num = &group->Num;
			}
			if (plan.Aircraft == nullptr) plan.Aircraft = group->Aircraft;
		}
	}
	return found;
}


void SuperWeaponKeys::ParaGroup::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
	stream.Serialize(Plane);
	stream.Serialize(Types);
	stream.Serialize(Num);
	stream.Serialize(Aircraft);
	stream.Serialize(IsSet);
}


/// <summary>
/// Lists the members these keys carry.
/// </summary>
void SuperWeaponKeys::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(MoneyAmount);
	stream.Serialize(Deferment);
	stream.Serialize(Shots);
	stream.Serialize(MaxCount);
	stream.Serialize(Group);
	stream.Serialize(InitialReady);
	stream.Serialize(VirtualCharge);
	stream.Serialize(AlwaysGranted);
	stream.Serialize(AllowPlayer);
	stream.Serialize(AllowAI);
	stream.Serialize(Unstoppable);
	stream.Serialize(ChargeToDrainRatio);
	stream.Serialize(AuxBuildings);
	stream.Serialize(NegBuildings);
	stream.Serialize(RequiredHouses);
	stream.Serialize(ForbiddenHouses);
	stream.Serialize(AffectsHouse);
	stream.Serialize(AffectsHouseSet);
	stream.Serialize(AffectsTarget);
	stream.Serialize(AffectsTargetSet);
	stream.Serialize(RequiresTarget);
	stream.Serialize(RequiresHouse);
	stream.Serialize(FireIntoShroud);
	stream.Serialize(RangeMinimum);
	stream.Serialize(RangeMaximum);
	stream.Serialize(AreaIsSet);
	stream.Serialize(AreaIsBlock);
	stream.Serialize(AreaRadius);
	stream.Serialize(AreaWidth);
	stream.Serialize(AreaHeight);
	stream.Serialize(Designators);
	stream.Serialize(Inhibitors);
	stream.Serialize(AnyDesignator);
	stream.Serialize(AnyInhibitor);
	stream.Serialize(AITargeting);
	stream.Serialize(AIRequiresTarget);
	stream.Serialize(AIRequiresHouse);
	stream.Serialize(AIConstraints);
	stream.Serialize(AIPreference);
	stream.Serialize(UseAITargeting);
	stream.Serialize(AutoFire);
	stream.Serialize(ManualFire);
	stream.Serialize(ShowCameo);
	stream.Serialize(TimerVisibility);
	stream.Serialize(Warhead);
	stream.Serialize(Damage);
	stream.Serialize(Animation);
	stream.Serialize(AnimationHeight);
	stream.Serialize(Sound);
	stream.Serialize(ActivationSound);
	stream.Serialize(CreateRadarEvent);
	stream.Serialize(EvaReady);
	stream.Serialize(EvaDetected);
	stream.Serialize(EvaActivated);
	stream.Serialize(EvaImpatient);
	stream.Serialize(EvaSelectTarget);
	stream.Serialize(EvaInsufficientFunds);
	stream.Serialize(MessageDetected);
	stream.Serialize(MessageReady);
	stream.Serialize(MessageLaunch);
	stream.Serialize(MessageActivate);
	stream.Serialize(MessageAbort);
	stream.Serialize(MessageInsufficientFunds);
	stream.Serialize(MessageCannotFire);
	stream.Serialize(MessageFirerColor);
	stream.Serialize(TextHold);
	stream.Serialize(TextReady);
	stream.Serialize(TextCharging);
	stream.Serialize(TextActive);
	stream.Serialize(TextPreparing);
	stream.Serialize(Cursor);
	stream.Serialize(NoCursor);
	stream.Serialize(LightEnabled);
	stream.Serialize(LightAmbient);
	stream.Serialize(LightRed);
	stream.Serialize(LightGreen);
	stream.Serialize(LightBlue);
	stream.Serialize(DeliverTypes);
	stream.Serialize(DeliverOwnerMode);
	stream.Serialize(DeliverBaseNormal);
	stream.Serialize(DeliverBuildups);
	stream.Serialize(DropPodTypes);
	stream.Serialize(DropPodMinimum);
	stream.Serialize(DropPodMaximum);
	stream.Serialize(DropPodVeterancy);
	stream.Serialize(HunterSeekerBuildings);
	stream.Serialize(HunterSeekerType);
	stream.Serialize(HunterSeekerRandomOnly);
	stream.Serialize(EMPulseCannons);
	stream.Serialize(EMPulseTargetSelf);
	stream.Serialize(EMPulseLinked);
	stream.Serialize(EMPulsePulseBall);
	stream.Serialize(EMPulseNoPulseBall);
	stream.Serialize(EMPulsePulseDelay);
	stream.Serialize(ProtectDuration);
	stream.Serialize(ProtectPowerOutage);
	stream.Serialize(ProtectFadeSoundTime);
	stream.Serialize(NukePayload);
	stream.Serialize(NukeSiloLaunch);
	stream.Serialize(LightningDuration);
	stream.Serialize(LightningRadarOutage);
	stream.Serialize(LightningRadarOutageAffects);
	stream.Serialize(LightningHitDelay);
	stream.Serialize(LightningScatterDelay);
	stream.Serialize(LightningScatterCount);
	stream.Serialize(LightningSeparation);
	stream.Serialize(LightningPrintText);
	stream.Serialize(LightningDebrisMin);
	stream.Serialize(LightningDebrisMax);
	stream.Serialize(LightningCloudHeight);
	stream.Serialize(LightningBoltExplosion);
	stream.Serialize(LightningSounds);
	stream.Serialize(LightningClouds);
	stream.Serialize(LightningBolts);
	stream.Serialize(LightningDebris);
	stream.Serialize(LightningSoundsSet);
	stream.Serialize(LightningCloudsSet);
	stream.Serialize(LightningBoltsSet);
	stream.Serialize(LightningDebrisSet);
	stream.Serialize(LightningIgnoreRod);
	stream.Serialize(DominatorFirstAnim);
	stream.Serialize(DominatorSecondAnim);
	stream.Serialize(DominatorControlAnim);
	stream.Serialize(DominatorFirstAnimHeight);
	stream.Serialize(DominatorSecondAnimHeight);
	stream.Serialize(DominatorFireAtPercentage);
	stream.Serialize(DominatorCapture);
	stream.Serialize(DominatorRipple);
	stream.Serialize(DominatorCaptureMindControlled);
	stream.Serialize(DominatorCapturePermaMindControlled);
	stream.Serialize(DominatorCaptureImmuneToPsionics);
	stream.Serialize(DominatorPermanentCapture);
	stream.Serialize(ChronoBlastSrc);
	stream.Serialize(ChronoBlastDest);
	stream.Serialize(ChronoKillOrganic);
	stream.Serialize(ChronoKillTeleporters);
	stream.Serialize(ChronoAffectsIronCurtain);
	stream.Serialize(ChronoAffectsUnwarpable);
	stream.Serialize(ChronoKillCargo);
	stream.Serialize(ChronoReconsiderBuildings);
	stream.Serialize(ChronoAffectsUndeployable);
	stream.Serialize(ChronoBlowUnplaceable);
	stream.Serialize(PostDependent);
	stream.Serialize(MutateExplosion);
	stream.Serialize(MutateIgnoreCyborg);
	stream.Serialize(MutateIgnoreNotHuman);
	stream.Serialize(MutateKillNatural);
	stream.Serialize(SonarPulseDelay);
	stream.Serialize(BatteryPower);
	stream.Serialize(BatteryOverpower);
	stream.Serialize(BatteryKeepOnline);
	stream.Serialize(SpyPlaneType);
	stream.Serialize(SpyPlaneCount);
	stream.Serialize(SpyPlaneMission);
	stream.Serialize(ParaDefault);
	stream.Serialize(ParaByOwner);
	stream.Serialize(ParaDropCount);
}


/// <summary>
/// Submits the keys that change play to the game state checksum, so the multiplayer sync check shows
/// two machines that read different rules. Keys that only change what one player sees are left out.
/// </summary>
void SuperWeaponKeys::Compute_CRC(CRCEngine & crc) const
{
	crc(MoneyAmount);
	crc(Deferment);
	crc(Shots);
	crc(MaxCount);
	crc(Group);
	crc(InitialReady);
	crc(VirtualCharge);
	crc(AlwaysGranted);
	crc(AllowPlayer);
	crc(AllowAI);
	crc((int)AffectsHouse);
	crc((int)AffectsTarget);
	crc((int)RequiresTarget);
	crc((int)RequiresHouse);
	crc(AreaIsSet);
	crc(AreaWidth);
	crc(AreaHeight);
	crc(Damage);
	crc(AITargeting);
	crc((int)AIConstraints);
	crc(AIPreference);
	crc(UseAITargeting);
	crc(AutoFire);
	crc(DeliverOwnerMode);
	crc(DropPodMinimum);
	crc(DropPodMaximum);
	crc(EMPulsePulseDelay);
	crc(ProtectDuration);
	crc(ProtectPowerOutage);
	crc(ProtectFadeSoundTime);
	crc(SonarPulseDelay);
	crc(LightEnabled);
	crc(LightAmbient);
	crc(LightRed);
	crc(LightGreen);
	crc(LightBlue);
	crc(LightningDuration);
	crc(LightningRadarOutage);
	crc((int)LightningRadarOutageAffects);
	crc(LightningHitDelay);
	crc(LightningScatterDelay);
	crc(LightningScatterCount);
	crc(LightningSeparation);
	crc(LightningPrintText);
	crc(LightningDebrisMin);
	crc(LightningDebrisMax);
	crc(LightningCloudHeight);
	crc(LightningSounds.Count());
	crc(LightningClouds.Count());
	crc(LightningBolts.Count());
	crc(LightningDebris.Count());
	crc(DominatorFirstAnimHeight);
	crc(DominatorSecondAnimHeight);
	crc(DominatorFireAtPercentage);
	crc(DominatorCapture);
	crc(DominatorCaptureMindControlled);
	crc(DominatorCapturePermaMindControlled);
	crc(DominatorCaptureImmuneToPsionics);
	crc(DominatorPermanentCapture);
	crc(ChronoKillOrganic);
	crc(ChronoKillTeleporters);
	crc(ChronoAffectsIronCurtain);
	crc(ChronoAffectsUnwarpable);
	crc(ChronoKillCargo);
	crc(ChronoReconsiderBuildings);
	crc(ChronoAffectsUndeployable);
	crc(ChronoBlowUnplaceable);
	crc(MutateExplosion);
	crc(MutateIgnoreCyborg);
	crc(MutateIgnoreNotHuman);
	crc(MutateKillNatural);
	crc(LightningIgnoreRod);
	crc(BatteryPower);
	crc(SpyPlaneCount);
	crc(SpyPlaneMission);
	crc(ParaDropCount);
	crc(DeliverTypes.Count());
	crc(Designators.Count());
	crc(Inhibitors.Count());
}


bool SuperWeaponKeys::Has_Target_Rules(void) const
{
	return(RequiresTarget != 0 || RequiresHouse != 0 || RangeMinimum >= 0.0f || RangeMaximum >= 0.0f || Designators.Count() > 0 || Inhibitors.Count() > 0 || AnyDesignator || AnyInhibitor);
}


SuperRules::Area SuperWeaponKeys::Target_Area(void) const
{
	SuperRules::Area area;
	area.IsSet = AreaIsSet;
	area.IsBlock = AreaIsBlock;
	area.Radius = AreaRadius;
	area.Width = AreaWidth;
	area.Height = AreaHeight;
	return(area);
}


SuperRules::Area SuperWeaponKeys::Target_Area_Or_Block(int width, int height) const
{
	SuperRules::Area area = Target_Area();
	if (!area.IsSet) {
		area.IsSet = true;
		area.IsBlock = true;
		area.Width = width;
		area.Height = height;
	}
	return(area);
}


unsigned SuperWeaponKeys::Affected_Targets(unsigned fallback) const
{
	return(AffectsTargetSet ? AffectsTarget : fallback);
}


bool SuperWeaponKeys::Allows_Cannon(BuildingTypeClass const * type) const
{
	if (EMPulseCannons.Count() == 0) {
		return(true);
	}
	for (int index = 0; index < EMPulseCannons.Count(); index++) {
		if (EMPulseCannons[index] == type) {
			return(true);
		}
	}
	return(false);
}
