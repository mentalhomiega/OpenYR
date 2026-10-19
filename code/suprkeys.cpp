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


template<class T>
void Read_Type_List(CCINIClass const & ini, char const * section, char const * key, DynamicVectorClass<T *> const & source, TypeList<T const *> & list)
{
	if (!ini.Is_Present(section, key)) {
		return;
	}
	list.Clear();
	for (std::string const & name : Split_List(ini.Get_String(section, key))) {
		T const * type = Find_Named(source, name.c_str());
		if (type != NULL) {
			list.Add(type);
		} else if (!Same(name, "none")) {
			DebugString("[%s] %s names no such type: %s; skipped.\n", section, key, name.c_str());
		}
	}
}


void Read_Techno_List(CCINIClass const & ini, char const * section, char const * key, TypeList<TechnoTypeClass const *> & list)
{
	Read_Type_List<TechnoTypeClass>(ini, section, key, TechnoTypes, list);
}


void Read_Text(CCINIClass const & ini, char const * section, char const * key, std::string & text)
{
	if (ini.Is_Present(section, key)) {
		std::string const value = ini.Get_String(section, key);
		text = std::string(Trim(value));
	}
}


void Read_Mask(CCINIClass const & ini, char const * section, char const * key, bool houses, unsigned & mask)
{
	if (ini.Is_Present(section, key)) {
		std::string const value = ini.Get_String(section, key);
		mask = houses ? Parse_Houses(value, mask) : Parse_Targets(value, mask);
	}
}

}


SuperWeaponKeys::SuperWeaponKeys(void) :
	MoneyAmount(0),
	Deferment(-1),
	Shots(-1),
	MaxCount(1),
	Group(0),
	InitialReady(false),
	AllowPlayer(true),
	AllowAI(true),
	Unstoppable(false),
	ChargeToDrainRatio(0.0f),
	AffectsHouse(HOUSE_ALL),
	AffectsHouseSet(false),
	AffectsTarget(0),
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
	UseAITargeting(false),
	AutoFire(false),
	ManualFire(true),
	ShowCameo(true),
	Warhead(NULL),
	Damage(-1),
	Animation(NULL),
	AnimationHeight(5),
	Sound(VOC_NONE),
	ActivationSound(VOC_NONE),
	CreateRadarEvent(false),
	MessageFirerColor(false),
	LightEnabled(false),
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
	NukePayload(NULL),
	NukeSiloLaunch(true),
	SonarPulseDelay(60),
	BatteryPower(0),
	SpyPlaneType(NULL),
	SpyPlaneCount(1),
	ParaDropCount(1)
{
}


/// <summary>
/// Reads the Ares superweapon keys from the section. A key the section leaves out keeps the value the
/// weapon already has, so a rules file read over another changes only the keys it names.
/// </summary>
void SuperWeaponKeys::Read_INI(CCINIClass const & ini, char const * section)
{
	MoneyAmount = ini.Get_Int(section, "Money.Amount", MoneyAmount);
	Deferment = ini.Get_Int(section, "SW.Deferment", Deferment);
	Shots = ini.Get_Int(section, "SW.Shots", Shots);
	MaxCount = ini.Get_Int(section, "SW.MaxCount", MaxCount);
	Group = ini.Get_Int(section, "SW.Group", Group);
	InitialReady = ini.Get_Bool(section, "SW.InitialReady", InitialReady);
	AllowPlayer = ini.Get_Bool(section, "SW.AllowPlayer", AllowPlayer);
	AllowAI = ini.Get_Bool(section, "SW.AllowAI", AllowAI);
	Unstoppable = ini.Get_Bool(section, "SW.Unstoppable", Unstoppable);
	ChargeToDrainRatio = float(ini.Get_Float(section, "SW.ChargeToDrainRatio", ChargeToDrainRatio));

	Read_Type_List<BuildingTypeClass>(ini, section, "SW.AuxBuildings", BuildingTypes, AuxBuildings);
	Read_Type_List<BuildingTypeClass>(ini, section, "SW.NegBuildings", BuildingTypes, NegBuildings);
	RequiredHouses = ini.Get_House_List(section, "SW.RequiredHouses", RequiredHouses);
	ForbiddenHouses = ini.Get_House_List(section, "SW.ForbiddenHouses", ForbiddenHouses);

	Read_Mask(ini, section, "SW.AffectsHouse", true, AffectsHouse);
	AffectsHouseSet = AffectsHouseSet || ini.Is_Present(section, "SW.AffectsHouse");
	Read_Mask(ini, section, "SW.AffectsTarget", false, AffectsTarget);
	Read_Mask(ini, section, "SW.RequiresTarget", false, RequiresTarget);
	Read_Mask(ini, section, "SW.RequiresHouse", true, RequiresHouse);
	FireIntoShroud = ini.Get_Bool(section, "SW.FireIntoShroud", FireIntoShroud);
	RangeMinimum = float(ini.Get_Float(section, "SW.RangeMinimum", RangeMinimum));
	RangeMaximum = float(ini.Get_Float(section, "SW.RangeMaximum", RangeMaximum));
	if (ini.Is_Present(section, "SW.Range")) {
		Area const area = Parse_Area(ini.Get_String(section, "SW.Range"));
		if (area.IsSet) {
			AreaIsSet = true;
			AreaIsBlock = area.IsBlock;
			AreaRadius = area.Radius;
			AreaWidth = area.Width;
			AreaHeight = area.Height;
		}
	}
	Read_Techno_List(ini, section, "SW.Designators", Designators);
	Read_Techno_List(ini, section, "SW.Inhibitors", Inhibitors);
	AnyDesignator = ini.Get_Bool(section, "SW.AnyDesignator", AnyDesignator);
	AnyInhibitor = ini.Get_Bool(section, "SW.AnyInhibitor", AnyInhibitor);

	if (ini.Is_Present(section, "SW.AITargeting")) {
		AITarget const mode = Parse_AI_Target(ini.Get_String(section, "SW.AITargeting"));
		if (mode != AITarget::Default) {
			AITargeting = int(mode);
		}
	}
	if (ini.Is_Present(section, "SW.AIRequiresTarget")) {
		AIRequiresTarget = Parse_Targets(ini.Get_String(section, "SW.AIRequiresTarget"), AIRequiresTarget);
	}
	if (ini.Is_Present(section, "SW.AIRequiresHouse")) {
		std::string const value = ini.Get_String(section, "SW.AIRequiresHouse");
		unsigned flags = 0;
		bool found = false;
		for (std::string const & name : Split_List(value)) {
			if (Same(name, "others")) { flags |= HOUSE_ALLIES | HOUSE_ENEMIES; found = true; }
			else {
				unsigned const one = Parse_Houses(name, UNSET_MASK);
				if (one != UNSET_MASK) { flags |= one; found = true; }
			}
		}
		if (found) AIRequiresHouse = flags;
	}
	if (ini.Is_Present(section, "SW.AITargeting.Constraints")) {
		AIConstraints = Parse_Constraints(ini.Get_String(section, "SW.AITargeting.Constraints"));
	}
	UseAITargeting = ini.Get_Bool(section, "SW.UseAITargeting", UseAITargeting);
	AutoFire = ini.Get_Bool(section, "SW.AutoFire", AutoFire);
	ManualFire = ini.Get_Bool(section, "SW.ManualFire", ManualFire);
	ShowCameo = ini.Get_Bool(section, "SW.ShowCameo", ShowCameo);

	if (ini.Is_Present(section, "SW.Warhead")) {
		Warhead = Find_Named(Warheads, ini.Get_String(section, "SW.Warhead").c_str());
	}
	Damage = ini.Get_Int(section, "SW.Damage", Damage);
	if (ini.Is_Present(section, "SW.Animation")) {
		Animation = Find_Named(AnimTypes, ini.Get_String(section, "SW.Animation").c_str());
	}
	AnimationHeight = ini.Get_Int(section, "SW.AnimationHeight", AnimationHeight);
	Sound = ini.Get_VocType(section, "SW.Sound", Sound);
	ActivationSound = ini.Get_VocType(section, "SW.ActivationSound", ActivationSound);
	CreateRadarEvent = ini.Get_Bool(section, "SW.CreateRadarEvent", CreateRadarEvent);

	Read_Text(ini, section, "EVA.Ready", EvaReady);
	Read_Text(ini, section, "EVA.Detected", EvaDetected);
	Read_Text(ini, section, "EVA.Activated", EvaActivated);
	Read_Text(ini, section, "EVA.Impatient", EvaImpatient);
	Read_Text(ini, section, "EVA.SelectTarget", EvaSelectTarget);
	Read_Text(ini, section, "EVA.InsufficientFunds", EvaInsufficientFunds);
	Read_Text(ini, section, "Message.Detected", MessageDetected);
	Read_Text(ini, section, "Message.Ready", MessageReady);
	Read_Text(ini, section, "Message.Launch", MessageLaunch);
	Read_Text(ini, section, "Message.Activate", MessageActivate);
	Read_Text(ini, section, "Message.Abort", MessageAbort);
	Read_Text(ini, section, "Message.InsufficientFunds", MessageInsufficientFunds);
	Read_Text(ini, section, "Message.CannotFire", MessageCannotFire);
	MessageFirerColor = ini.Get_Bool(section, "Message.FirerColor", MessageFirerColor);
	Read_Text(ini, section, "Text.Hold", TextHold);
	Read_Text(ini, section, "Text.Ready", TextReady);
	Read_Text(ini, section, "Text.Charging", TextCharging);
	Read_Text(ini, section, "Text.Active", TextActive);
	Read_Text(ini, section, "Text.Preparing", TextPreparing);
	Read_Text(ini, section, "Cursor", Cursor);
	Read_Text(ini, section, "NoCursor", NoCursor);

	LightEnabled = ini.Get_Bool(section, "Light.Enabled", LightEnabled);
	LightAmbient = ini.Get_Int(section, "Light.Ambient", LightAmbient);
	LightRed = ini.Get_Int(section, "Light.Red", LightRed);
	LightGreen = ini.Get_Int(section, "Light.Green", LightGreen);
	LightBlue = ini.Get_Int(section, "Light.Blue", LightBlue);

	Read_Techno_List(ini, section, "Deliver.Types", DeliverTypes);
	if (ini.Is_Present(section, "Deliver.Owner")) {
		DeliverOwnerMode = int(Parse_Deliver_Owner(ini.Get_String(section, "Deliver.Owner")));
	}
	DeliverBaseNormal = ini.Get_Bool(section, "Deliver.BaseNormal", DeliverBaseNormal);
	DeliverBuildups = ini.Get_Bool(section, "Deliver.Buildups", DeliverBuildups);

	Read_Techno_List(ini, section, "DropPod.Types", DropPodTypes);
	DropPodMinimum = ini.Get_Int(section, "DropPod.Minimum", DropPodMinimum);
	DropPodMaximum = ini.Get_Int(section, "DropPod.Maximum", DropPodMaximum);
	DropPodVeterancy = float(ini.Get_Float(section, "DropPod.Veterancy", DropPodVeterancy));

	Read_Type_List<BuildingTypeClass>(ini, section, "HunterSeeker.Buildings", BuildingTypes, HunterSeekerBuildings);
	if (ini.Is_Present(section, "HunterSeeker.Type")) {
		HunterSeekerType = Find_Named(UnitTypes, ini.Get_String(section, "HunterSeeker.Type").c_str());
	}
	HunterSeekerRandomOnly = ini.Get_Bool(section, "HunterSeeker.RandomOnly", HunterSeekerRandomOnly);

	Read_Type_List<BuildingTypeClass>(ini, section, "EMPulse.Cannons", BuildingTypes, EMPulseCannons);
	EMPulseTargetSelf = ini.Get_Bool(section, "EMPulse.TargetSelf", EMPulseTargetSelf);
	EMPulseLinked = ini.Get_Bool(section, "EMPulse.Linked", EMPulseLinked);
	if (ini.Is_Present(section, "EMPulse.PulseBall")) {
		std::string const name = ini.Get_String(section, "EMPulse.PulseBall");
		EMPulseNoPulseBall = Same(Trim(name), "none");
		EMPulsePulseBall = EMPulseNoPulseBall ? NULL : Find_Named(AnimTypes, std::string(Trim(name)).c_str());
	}
	EMPulsePulseDelay = ini.Get_Int(section, "EMPulse.PulseDelay", EMPulsePulseDelay);

	if (ini.Is_Present(section, "Nuke.Payload")) {
		NukePayload = Find_Named(Weapons, ini.Get_String(section, "Nuke.Payload").c_str());
	}
	NukeSiloLaunch = ini.Get_Bool(section, "Nuke.SiloLaunch", NukeSiloLaunch);

	SonarPulseDelay = ini.Get_Int(section, "SonarPulse.Delay", SonarPulseDelay);

	BatteryPower = ini.Get_Int(section, "Battery.Power", BatteryPower);
	Read_Type_List<BuildingTypeClass>(ini, section, "Battery.Overpower", BuildingTypes, BatteryOverpower);
	Read_Type_List<BuildingTypeClass>(ini, section, "Battery.KeepOnline", BuildingTypes, BatteryKeepOnline);

	if (ini.Is_Present(section, "SpyPlane.Type")) {
		SpyPlaneType = Find_Named(AircraftTypes, ini.Get_String(section, "SpyPlane.Type").c_str());
	}
	SpyPlaneCount = ini.Get_Int(section, "SpyPlane.Count", SpyPlaneCount);

	ParaDropCount = ini.Get_Int(section, "ParaDrop.Count", ParaDropCount);
	auto read_group = [&](ParaGroup & group, std::string const & prefix) {
		std::string const types_key = prefix + "Types";
		std::string const num_key = prefix + "Num";
		std::string const aircraft_key = prefix + "Aircraft";
		if (ini.Is_Present(section, types_key.c_str())) {
			Read_Techno_List(ini, section, types_key.c_str(), group.Types);
			group.IsSet = true;
		}
		if (ini.Is_Present(section, num_key.c_str())) {
			group.Num = ini.Get_IntList(section, num_key.c_str(), group.Num);
			group.IsSet = true;
		}
		if (ini.Is_Present(section, aircraft_key.c_str())) {
			group.Aircraft = Find_Named(AircraftTypes, ini.Get_String(section, aircraft_key.c_str()).c_str());
			group.IsSet = true;
		}
	};
	read_group(ParaDefault, "ParaDrop.");
	auto read_owner = [&](char const * name) {
		std::string const prefix = std::string("ParaDrop.") + name + ".";
		ParaGroup group;
		group.Owner = name;
		for (ParaGroup const & old : ParaByOwner) {
			if (Same(old.Owner, name)) group = old;
		}
		read_group(group, prefix);
		if (!group.IsSet) return;
		for (ParaGroup & old : ParaByOwner) {
			if (Same(old.Owner, name)) { old = group; return; }
		}
		ParaByOwner.push_back(group);
	};
	for (int index = 0; index < HouseTypes.Count(); index++) {
		read_owner(HouseTypes[index]->Name());
	}
	for (int index = 0; index < Sides.Count(); index++) {
		read_owner(Sides[index]->Name());
	}
}


void SuperWeaponKeys::ParaGroup::Serialize(SaveStreamClass & stream)
{
	stream.Serialize(Owner);
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
	stream.Serialize(UseAITargeting);
	stream.Serialize(AutoFire);
	stream.Serialize(ManualFire);
	stream.Serialize(ShowCameo);
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
	stream.Serialize(NukePayload);
	stream.Serialize(NukeSiloLaunch);
	stream.Serialize(SonarPulseDelay);
	stream.Serialize(BatteryPower);
	stream.Serialize(BatteryOverpower);
	stream.Serialize(BatteryKeepOnline);
	stream.Serialize(SpyPlaneType);
	stream.Serialize(SpyPlaneCount);
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
	crc(UseAITargeting);
	crc(AutoFire);
	crc(DeliverOwnerMode);
	crc(DropPodMinimum);
	crc(DropPodMaximum);
	crc(EMPulsePulseDelay);
	crc(SonarPulseDelay);
	crc(BatteryPower);
	crc(SpyPlaneCount);
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
