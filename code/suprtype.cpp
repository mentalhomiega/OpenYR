/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "suprtype.h"

#include "_map.h"
#include "_mixfile.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "mixfile.h"
#include "mission.h"
#include "mouse.h"
#include "savestream.h"
#include "sun.h"
#include "swizzle.h"
#include "dbgprint.h"
#include "weapon.h"

#include <set>
#include <string>


/// <summary>
/// Converts a super weapon behavior name into its type number.
/// The behavior name says which hard coded effect a super weapon delivers -- the ion
/// cannon, the multi missile, and so on -- rather than which rules section declared it.
/// </summary>
/// <returns>Returns with the super weapon type named. Otherwise, SUPER_NONE is
/// returned.</returns>
bool SuperWeaponTypeClass::BatteryTypesExist = false;


SuperWeaponType Special_From_Name(char const * name)
{
	static const char * _names[SUPER_COUNT] = {
		"MultiMissile",
		"EMPulse",
		"Firestorm",
		"IonCannon",
		"HunterSeeker",
		"ChemMissile",
		"DropPod",
		"IronCurtain",
		"LightningStorm",
		"ChronoSphere",
		"ChronoWarp",
		"ParaDrop",
		"AmerParaDrop",
		"PsychicDominator",
		"SpyPlane",
		"GeneticConverter",
		"ForceShield",
		"PsychicReveal",
		"GenericWarhead",
		"UnitDelivery",
		"SonarPulse",
		"Battery"
	};

	for (int i = SUPER_FIRST; i < SUPER_COUNT; i++) {
		if (strcmpi(_names[i], name) == 0) {
			return(SuperWeaponType)(i);
		}
	}
	return(SUPER_NONE);
}


/// <summary>
/// Constructs a super weapon type of the rules name specified.
/// The new type is given a heap identifier and added to the super weapon type heap. Only
/// harmless defaults are installed here; Read_INI supplies the real characteristics.
/// </summary>
/// <param name="ininame">The rules section this super weapon is declared by.</param>
SuperWeaponTypeClass::SuperWeaponTypeClass(char const * ininame) :
	BASECLASS(ininame),
	VoxRecharge(VOX_NONE),
	VoxCharging(VOX_NONE),
	VoxImpatient(VOX_NONE),
	VoxSuspend(VOX_NONE),
	Type(SUPER_NONE),
	Weapon(NULL),
	RechargeTime(4500),
	CameoData(NULL),
	CameoSortOrder(0),
	Action(ACTION_NONE),
	StartSound(VOC_NONE),
	SpecialSound(VOC_NONE),
	AuxBuilding(NULL),
	SidebarImage(),
	UseChargeDrain(false),
	AIDefendAgainst(false),
	IsPowered(true),
	IsManualControl(false),
	IsDisableableFromShell(false),
	FlashSidebarTabFrames(-1),
	IsPreClick(false),
	IsPostClick(false),
	IsShowTimer(false),
	PreDependent(SUPER_NONE),
	Range(0.0f),
	LineMultiplier(0)
{
	Create_ID();
	HeapID = (SuperWeaponType)SuperWeaponTypes.Count();
	SuperWeaponTypes.Add(this);
	SidebarImage = IniName;
}


/// <summary>
/// Removes this super weapon type from the game.
/// The type is unlinked from the super weapon type heap so that nothing can reach it
/// afterwards.
/// </summary>
SuperWeaponTypeClass::~SuperWeaponTypeClass(void)
{
	SuperWeaponTypes.Delete(this);
}


ClassID SuperWeaponTypeClass::Class_ID(void) const
{
	return(ClassID_SuperWeaponTypeClass);
}


/// <summary>
/// Re-attaches the artwork this super weapon type names.
/// The sidebar cameo is fetched from the mix files again once the members have been
/// read, since artwork is never written to a save game.
/// </summary>
void SuperWeaponTypeClass::Post_Load(void)
{
	BASECLASS::Post_Load();

	char fullname[_MAX_FNAME+_MAX_EXT];
	_makepath(fullname, NULL, NULL, SidebarImage, ".SHP");
	CameoData = (ShapeSet *)MFCD::Retrieve(fullname);
	if (CameoData == NULL) {
		CameoData = (ShapeSet *)MFCD::Retrieve("XXICON.SHP");
	}
}


/// <summary>
/// Lists the members this super weapon type carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void SuperWeaponTypeClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(HeapID);
	stream.Serialize(Weapon);
	stream.Serialize(VoxRecharge);
	stream.Serialize(VoxCharging);
	stream.Serialize(VoxImpatient);
	stream.Serialize(VoxSuspend);
	stream.Serialize(RechargeTime);
	stream.Serialize(Type);
	// CameoData -- artwork, fetched from the mix files again as this loads.
	stream.Serialize(CameoSortOrder);
	stream.Serialize(Action);
	stream.Serialize(StartSound);
	stream.Serialize(SpecialSound);
	stream.Serialize(AuxBuilding);
	stream.Serialize(SidebarImage);
	stream.Serialize(SidebarPCX);
	stream.Serialize(UseChargeDrain);
	stream.Serialize(AIDefendAgainst);
	stream.Serialize(IsPowered);
	stream.Serialize(IsManualControl);
	stream.Serialize(IsDisableableFromShell);
	stream.Serialize(FlashSidebarTabFrames);
	stream.Serialize(IsPreClick);
	stream.Serialize(IsPostClick);
	stream.Serialize(IsShowTimer);
	stream.Serialize(PreDependent);
	stream.Serialize(Range);
	stream.Serialize(LineMultiplier);
	stream.Serialize(Keys);
}


/// <summary>
/// Fetches the run time type of this object.
/// </summary>
/// <returns>Returns with RTTI_SUPERWEAPONTYPE.</returns>
RTTIType SuperWeaponTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_SUPERWEAPONTYPE);
}


/// <summary>
/// Submits this super weapon type to the game state checksum.
/// This routine is used by the multiplayer sync check to prove that every machine is
/// playing with the same rules.
/// </summary>
void SuperWeaponTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(HeapID);
	crc(Action);
	crc(VoxRecharge);
	crc(VoxCharging);
	crc(VoxImpatient);
	crc(VoxSuspend);
	crc(IsPowered);
	crc(RechargeTime);
	crc(Type);
	crc(UseChargeDrain);
	crc(AIDefendAgainst);
	crc(IsManualControl);
	crc(IsDisableableFromShell);
	crc(FlashSidebarTabFrames);
	crc(IsPreClick);
	crc(IsPostClick);
	crc(PreDependent);
	Keys.Compute_CRC(crc);
}


/// <summary>
/// Fetches the heap identifier of this super weapon type.
/// </summary>
/// <returns>Returns with the index of this object within the super weapon type heap.</returns>
int SuperWeaponTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}


/// <summary>
/// Fetches this super weapon's characteristics from the rules.
/// This routine picks up the weapon, the announcement voices, the recharge delay, and
/// the sidebar cameo artwork. A missing cameo falls back on the generic icon so that the
/// sidebar always has something to draw.
/// </summary>
/// <returns>bool; Was the database entry found and read?</returns>
bool SuperWeaponTypeClass::Read_INI(CCINIClass const & ini)
{
	if (BASECLASS::Read_INI(ini)) {

		Weapon = TGet_Class(ini, IniName, "WeaponType", Weapon);

		VoxRecharge = ini.Get_VoxType(IniName, "RechargeVoice", VoxRecharge);
		VoxCharging = ini.Get_VoxType(IniName, "ChargingVoice", VoxCharging);
		VoxImpatient = ini.Get_VoxType(IniName, "ImpatientVoice", VoxImpatient);
		VoxSuspend = ini.Get_VoxType(IniName, "SuspendVoice", VoxSuspend);

		Action = ini.Get_ActionType(IniName, "Action", Action);
		StartSound = ini.Get_VocType(IniName, "StartSound", StartSound);
		SpecialSound = ini.Get_VocType(IniName, "SpecialSound", SpecialSound);
		IsPowered = ini.Get_Bool(IniName, "IsPowered", IsPowered);

		char buffer[40];
		ini.Get_String(IniName, "Type", "", buffer, sizeof(buffer));
		if (strlen(buffer) != 0) {
			SuperWeaponType type = Special_From_Name(buffer);
			if (type != SUPER_NONE) {
				Type = type;
			} else {
				// A name no behavior answers to leaves the weapon inert: it charges, shows and discharges without an effect.
				static std::set<std::string> reported;
				if (reported.insert(std::string(Name())).second) {
					DebugString("SuperWeaponType %s: Type=%s is not a behavior this engine implements; firing it does nothing.\n", Name(), buffer);
				}
			}
		}

		AuxBuilding = TGet_Class(ini, IniName, "AuxBuilding", AuxBuilding);
		UseChargeDrain = ini.Get_Bool(IniName, "UseChargeDrain", UseChargeDrain);
		AIDefendAgainst = ini.Get_Bool(IniName, "AIDefendAgainst", AIDefendAgainst);
		IsManualControl = ini.Get_Bool(IniName, "ManualControl", IsManualControl);
		IsDisableableFromShell = ini.Get_Bool(IniName, "DisableableFromShell", IsDisableableFromShell);
		FlashSidebarTabFrames = ini.Get_Int(IniName, "FlashSidebarTabFrames", FlashSidebarTabFrames);
		IsPreClick = ini.Get_Bool(IniName, "PreClick", IsPreClick);
		IsPostClick = ini.Get_Bool(IniName, "PostClick", IsPostClick);
		IsShowTimer = ini.Get_Bool(IniName, "ShowTimer", IsShowTimer);
		ini.Get_String(IniName, "PreDependent", "", buffer, sizeof(buffer));
		if (strlen(buffer) != 0) {
			SuperWeaponType type = Special_From_Name(buffer);
			if (type != SUPER_NONE) PreDependent = type;
		}
		CameoSortOrder = ini.Get_Int(IniName, "CameoSortOrder", CameoSortOrder);
		Range = (float)ini.Get_Float(IniName, "Range", Range);
		LineMultiplier = ini.Get_Int(IniName, "LineMultiplier", LineMultiplier);

		// The keys Ares adds. A key the section leaves out keeps the value the weapon already has.
		Keys.MoneyAmount = ini.Get_Int(IniName, "Money.Amount", Keys.MoneyAmount);
		Keys.Deferment = ini.Get_Int(IniName, "SW.Deferment", Keys.Deferment);
		Keys.Shots = ini.Get_Int(IniName, "SW.Shots", Keys.Shots);
		Keys.MaxCount = ini.Get_Int(IniName, "SW.MaxCount", Keys.MaxCount);
		Keys.Group = ini.Get_Int(IniName, "SW.Group", Keys.Group);
		Keys.InitialReady = ini.Get_Bool(IniName, "SW.InitialReady", Keys.InitialReady);
		Keys.AlwaysGranted = ini.Get_Bool(IniName, "SW.AlwaysGranted", Keys.AlwaysGranted);
		Keys.AllowPlayer = ini.Get_Bool(IniName, "SW.AllowPlayer", Keys.AllowPlayer);
		Keys.AllowAI = ini.Get_Bool(IniName, "SW.AllowAI", Keys.AllowAI);
		Keys.Unstoppable = ini.Get_Bool(IniName, "SW.Unstoppable", Keys.Unstoppable);
		Keys.ChargeToDrainRatio = (float)ini.Get_Float(IniName, "SW.ChargeToDrainRatio", Keys.ChargeToDrainRatio);

		SuperKeyParse::Building_List(ini.Get_String(IniName, "SW.AuxBuildings"), Keys.AuxBuildings);
		SuperKeyParse::Building_List(ini.Get_String(IniName, "SW.NegBuildings"), Keys.NegBuildings);
		Keys.RequiredHouses = ini.Get_House_List(IniName, "SW.RequiredHouses", Keys.RequiredHouses);
		Keys.ForbiddenHouses = ini.Get_House_List(IniName, "SW.ForbiddenHouses", Keys.ForbiddenHouses);

		Keys.AffectsHouse = SuperRules::Parse_Houses(ini.Get_String(IniName, "SW.AffectsHouse"), Keys.AffectsHouse);
		Keys.AffectsHouseSet = Keys.AffectsHouseSet || ini.Is_Present(IniName, "SW.AffectsHouse");
		Keys.AffectsTarget = SuperRules::Parse_Targets(ini.Get_String(IniName, "SW.AffectsTarget"), Keys.AffectsTarget);
		Keys.AffectsTargetSet = Keys.AffectsTargetSet || ini.Is_Present(IniName, "SW.AffectsTarget");
		Keys.RequiresTarget = SuperRules::Parse_Targets(ini.Get_String(IniName, "SW.RequiresTarget"), Keys.RequiresTarget);
		Keys.RequiresHouse = SuperRules::Parse_Houses(ini.Get_String(IniName, "SW.RequiresHouse"), Keys.RequiresHouse);
		Keys.FireIntoShroud = ini.Get_Bool(IniName, "SW.FireIntoShroud", Keys.FireIntoShroud);
		Keys.RangeMinimum = (float)ini.Get_Float(IniName, "SW.RangeMinimum", Keys.RangeMinimum);
		Keys.RangeMaximum = (float)ini.Get_Float(IniName, "SW.RangeMaximum", Keys.RangeMaximum);
		Keys.Set_Area(SuperRules::Parse_Area(ini.Get_String(IniName, "SW.Range")));
		SuperKeyParse::Techno_List(ini.Get_String(IniName, "SW.Designators"), Keys.Designators);
		SuperKeyParse::Techno_List(ini.Get_String(IniName, "SW.Inhibitors"), Keys.Inhibitors);
		Keys.AnyDesignator = ini.Get_Bool(IniName, "SW.AnyDesignator", Keys.AnyDesignator);
		Keys.AnyInhibitor = ini.Get_Bool(IniName, "SW.AnyInhibitor", Keys.AnyInhibitor);

		Keys.Set_AI_Targeting(ini.Get_String(IniName, "SW.AITargeting"));
		Keys.AIRequiresTarget = SuperRules::Parse_Targets(ini.Get_String(IniName, "SW.AIRequiresTarget"), Keys.AIRequiresTarget);
		Keys.Set_AI_Requires_House(ini.Get_String(IniName, "SW.AIRequiresHouse"));
		Keys.AIConstraints = SuperRules::Parse_Constraints_Or(ini.Get_String(IniName, "SW.AITargeting.Constraints"), Keys.AIConstraints);
		Keys.AIPreference = int(SuperRules::Parse_AI_Preference(ini.Get_String(IniName, "SW.AITargeting.Preference"), SuperRules::AIPreference(Keys.AIPreference)));
		Keys.UseAITargeting = ini.Get_Bool(IniName, "SW.UseAITargeting", Keys.UseAITargeting);
		Keys.AutoFire = ini.Get_Bool(IniName, "SW.AutoFire", Keys.AutoFire);
		Keys.ManualFire = ini.Get_Bool(IniName, "SW.ManualFire", Keys.ManualFire);
		Keys.ShowCameo = ini.Get_Bool(IniName, "SW.ShowCameo", Keys.ShowCameo);
		Keys.TimerVisibility = SuperRules::Parse_Houses(ini.Get_String(IniName, "SW.TimerVisibility"), Keys.TimerVisibility);

		Keys.Warhead = SuperKeyParse::Warhead_Named(ini.Get_String(IniName, "SW.Warhead"), Keys.Warhead);
		Keys.Damage = ini.Get_Int(IniName, "SW.Damage", Keys.Damage);
		Keys.Animation = SuperKeyParse::Anim_Named(ini.Get_String(IniName, "SW.Animation"), Keys.Animation);
		Keys.AnimationHeight = ini.Get_Int(IniName, "SW.AnimationHeight", Keys.AnimationHeight);
		Keys.Sound = ini.Get_VocType(IniName, "SW.Sound", Keys.Sound);
		Keys.ActivationSound = ini.Get_VocType(IniName, "SW.ActivationSound", Keys.ActivationSound);
		Keys.CreateRadarEvent = ini.Get_Bool(IniName, "SW.CreateRadarEvent", Keys.CreateRadarEvent);

		Keys.EvaReady = SuperKeyParse::Text_Or(ini.Get_String(IniName, "EVA.Ready"), Keys.EvaReady);
		Keys.EvaDetected = SuperKeyParse::Text_Or(ini.Get_String(IniName, "EVA.Detected"), Keys.EvaDetected);
		Keys.EvaActivated = SuperKeyParse::Text_Or(ini.Get_String(IniName, "EVA.Activated"), Keys.EvaActivated);
		Keys.EvaImpatient = SuperKeyParse::Text_Or(ini.Get_String(IniName, "EVA.Impatient"), Keys.EvaImpatient);
		Keys.EvaSelectTarget = SuperKeyParse::Text_Or(ini.Get_String(IniName, "EVA.SelectTarget"), Keys.EvaSelectTarget);
		Keys.EvaInsufficientFunds = SuperKeyParse::Text_Or(ini.Get_String(IniName, "EVA.InsufficientFunds"), Keys.EvaInsufficientFunds);
		Keys.MessageDetected = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Message.Detected"), Keys.MessageDetected);
		Keys.MessageReady = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Message.Ready"), Keys.MessageReady);
		Keys.MessageLaunch = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Message.Launch"), Keys.MessageLaunch);
		Keys.MessageActivate = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Message.Activate"), Keys.MessageActivate);
		Keys.MessageAbort = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Message.Abort"), Keys.MessageAbort);
		Keys.MessageInsufficientFunds = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Message.InsufficientFunds"), Keys.MessageInsufficientFunds);
		Keys.MessageCannotFire = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Message.CannotFire"), Keys.MessageCannotFire);
		Keys.MessageFirerColor = ini.Get_Bool(IniName, "Message.FirerColor", Keys.MessageFirerColor);
		Keys.TextHold = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Text.Hold"), Keys.TextHold);
		Keys.TextReady = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Text.Ready"), Keys.TextReady);
		Keys.TextCharging = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Text.Charging"), Keys.TextCharging);
		Keys.TextActive = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Text.Active"), Keys.TextActive);
		Keys.TextPreparing = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Text.Preparing"), Keys.TextPreparing);
		Keys.Cursor = SuperKeyParse::Text_Or(ini.Get_String(IniName, "Cursor"), Keys.Cursor);
		Keys.NoCursor = SuperKeyParse::Text_Or(ini.Get_String(IniName, "NoCursor"), Keys.NoCursor);

		Keys.LightEnabled = ini.Get_Bool(IniName, "Light.Enabled", Keys.LightEnabled);
		Keys.LightAmbient = ini.Get_Int(IniName, "Light.Ambient", Keys.LightAmbient);
		Keys.LightRed = ini.Get_Int(IniName, "Light.Red", Keys.LightRed);
		Keys.LightGreen = ini.Get_Int(IniName, "Light.Green", Keys.LightGreen);
		Keys.LightBlue = ini.Get_Int(IniName, "Light.Blue", Keys.LightBlue);

		SuperKeyParse::Techno_List(ini.Get_String(IniName, "Deliver.Types"), Keys.DeliverTypes);
		Keys.DeliverOwnerMode = SuperKeyParse::Deliver_Owner(ini.Get_String(IniName, "Deliver.Owner"), Keys.DeliverOwnerMode);
		Keys.DeliverBaseNormal = ini.Get_Bool(IniName, "Deliver.BaseNormal", Keys.DeliverBaseNormal);
		Keys.DeliverBuildups = ini.Get_Bool(IniName, "Deliver.Buildups", Keys.DeliverBuildups);

		SuperKeyParse::Techno_List(ini.Get_String(IniName, "DropPod.Types"), Keys.DropPodTypes);
		Keys.DropPodMinimum = ini.Get_Int(IniName, "DropPod.Minimum", Keys.DropPodMinimum);
		Keys.DropPodMaximum = ini.Get_Int(IniName, "DropPod.Maximum", Keys.DropPodMaximum);
		Keys.DropPodVeterancy = (float)ini.Get_Float(IniName, "DropPod.Veterancy", Keys.DropPodVeterancy);

		SuperKeyParse::Building_List(ini.Get_String(IniName, "HunterSeeker.Buildings"), Keys.HunterSeekerBuildings);
		Keys.HunterSeekerType = SuperKeyParse::Unit_Named(ini.Get_String(IniName, "HunterSeeker.Type"), Keys.HunterSeekerType);
		Keys.HunterSeekerRandomOnly = ini.Get_Bool(IniName, "HunterSeeker.RandomOnly", Keys.HunterSeekerRandomOnly);

		SuperKeyParse::Building_List(ini.Get_String(IniName, "EMPulse.Cannons"), Keys.EMPulseCannons);
		Keys.EMPulseTargetSelf = ini.Get_Bool(IniName, "EMPulse.TargetSelf", Keys.EMPulseTargetSelf);
		Keys.EMPulseLinked = ini.Get_Bool(IniName, "EMPulse.Linked", Keys.EMPulseLinked);
		Keys.Set_Pulse_Ball(ini.Get_String(IniName, "EMPulse.PulseBall"));
		Keys.EMPulsePulseDelay = ini.Get_Int(IniName, "EMPulse.PulseDelay", Keys.EMPulsePulseDelay);

		Keys.ProtectDuration = ini.Get_Int(IniName, "Protect.Duration", Keys.ProtectDuration);
		Keys.ProtectPowerOutage = ini.Get_Int(IniName, "Protect.PowerOutage", Keys.ProtectPowerOutage);
		Keys.ProtectFadeSoundTime = ini.Get_Int(IniName, "Protect.PlayFadeSoundTime", Keys.ProtectFadeSoundTime);

		Keys.NukePayload = SuperKeyParse::Weapon_Named(ini.Get_String(IniName, "Nuke.Payload"), Keys.NukePayload);
		Keys.NukeSiloLaunch = ini.Get_Bool(IniName, "Nuke.SiloLaunch", Keys.NukeSiloLaunch);
		SuperKeyParse::Anim_Override(ini.Get_String(IniName, "Nuke.TakeOff"), Keys.NukeTakeOff, Keys.NukeTakeOffSet);

		Keys.LightningDuration = ini.Get_Int(IniName, "Lightning.Duration", Keys.LightningDuration);
		Keys.LightningRadarOutage = ini.Get_Int(IniName, "Lightning.RadarOutage", Keys.LightningRadarOutage);
		Keys.LightningRadarOutageAffects = SuperRules::Parse_Houses(ini.Get_String(IniName, "Lightning.RadarOutageAffects"), Keys.LightningRadarOutageAffects);
		Keys.LightningHitDelay = ini.Get_Int(IniName, "Lightning.HitDelay", Keys.LightningHitDelay);
		Keys.LightningScatterDelay = ini.Get_Int(IniName, "Lightning.ScatterDelay", Keys.LightningScatterDelay);
		Keys.LightningScatterCount = ini.Get_Int(IniName, "Lightning.ScatterCount", Keys.LightningScatterCount);
		Keys.LightningSeparation = ini.Get_Int(IniName, "Lightning.Separation", Keys.LightningSeparation);
		if (ini.Is_Present(IniName, "Lightning.PrintText")) {
			Keys.LightningPrintText = ini.Get_Bool(IniName, "Lightning.PrintText", Keys.LightningPrintText > 0) ? 1 : 0;
		}
		Keys.LightningDebrisMin = ini.Get_Int(IniName, "Lightning.DebrisMin", Keys.LightningDebrisMin);
		Keys.LightningDebrisMax = ini.Get_Int(IniName, "Lightning.DebrisMax", Keys.LightningDebrisMax);
		Keys.LightningCloudHeight = ini.Get_Int(IniName, "Lightning.CloudHeight", Keys.LightningCloudHeight);
		Keys.LightningBoltExplosion = SuperKeyParse::Anim_Named(ini.Get_String(IniName, "Lightning.BoltExplosion"), Keys.LightningBoltExplosion);
		if (ini.Is_Present(IniName, "Lightning.Sounds")) {
			TypeList<int> const sounds = CCINIClass::Get_VocType_List(ini, IniName, "Lightning.Sounds", TypeList<int>());
			Keys.LightningSounds.Clear();
			for (int index = 0; index < sounds.Count(); index++) {
				if (sounds[index] != VOC_NONE) {
					Keys.LightningSounds.Add(sounds[index]);
				}
			}
			Keys.LightningSoundsSet = true;
		}
		SuperKeyParse::Anim_List(ini.Get_String(IniName, "Lightning.Clouds"), Keys.LightningClouds, Keys.LightningCloudsSet);
		SuperKeyParse::Anim_List(ini.Get_String(IniName, "Lightning.Bolts"), Keys.LightningBolts, Keys.LightningBoltsSet);
		SuperKeyParse::Anim_List(ini.Get_String(IniName, "Lightning.Debris"), Keys.LightningDebris, Keys.LightningDebrisSet);

		Keys.DominatorFirstAnim = SuperKeyParse::Anim_Named(ini.Get_String(IniName, "Dominator.FirstAnim"), Keys.DominatorFirstAnim);
		Keys.DominatorSecondAnim = SuperKeyParse::Anim_Named(ini.Get_String(IniName, "Dominator.SecondAnim"), Keys.DominatorSecondAnim);
		Keys.DominatorControlAnim = SuperKeyParse::Anim_Named(ini.Get_String(IniName, "Dominator.ControlAnim"), Keys.DominatorControlAnim);
		Keys.DominatorFirstAnimHeight = ini.Get_Int(IniName, "Dominator.FirstAnimHeight", Keys.DominatorFirstAnimHeight);
		Keys.DominatorSecondAnimHeight = ini.Get_Int(IniName, "Dominator.SecondAnimHeight", Keys.DominatorSecondAnimHeight);
		Keys.DominatorFireAtPercentage = ini.Get_Int(IniName, "Dominator.FireAtPercentage", Keys.DominatorFireAtPercentage);
		Keys.DominatorCapture = ini.Get_Bool(IniName, "Dominator.Capture", Keys.DominatorCapture);
		Keys.DominatorRipple = ini.Get_Bool(IniName, "Dominator.Ripple", Keys.DominatorRipple);
		Keys.DominatorCaptureMindControlled = ini.Get_Bool(IniName, "Dominator.CaptureMindControlled", Keys.DominatorCaptureMindControlled);
		Keys.DominatorCapturePermaMindControlled = ini.Get_Bool(IniName, "Dominator.CapturePermaMindControlled", Keys.DominatorCapturePermaMindControlled);
		Keys.DominatorCaptureImmuneToPsionics = ini.Get_Bool(IniName, "Dominator.CaptureImmuneToPsionics", Keys.DominatorCaptureImmuneToPsionics);
		Keys.DominatorPermanentCapture = ini.Get_Bool(IniName, "Dominator.PermanentCapture", Keys.DominatorPermanentCapture);

		Keys.ChronoBlastSrc = SuperKeyParse::Anim_Named(ini.Get_String(IniName, "Chronosphere.BlastSrc"), Keys.ChronoBlastSrc);
		Keys.ChronoBlastDest = SuperKeyParse::Anim_Named(ini.Get_String(IniName, "Chronosphere.BlastDest"), Keys.ChronoBlastDest);
		Keys.ChronoKillOrganic = ini.Get_Bool(IniName, "Chronosphere.KillOrganic", Keys.ChronoKillOrganic);
		Keys.ChronoKillTeleporters = ini.Get_Bool(IniName, "Chronosphere.KillTeleporters", Keys.ChronoKillTeleporters);
		Keys.ChronoAffectsIronCurtain = ini.Get_Bool(IniName, "Chronosphere.AffectsIronCurtain", Keys.ChronoAffectsIronCurtain);
		Keys.ChronoAffectsUnwarpable = ini.Get_Bool(IniName, "Chronosphere.AffectsUnwarpable", Keys.ChronoAffectsUnwarpable);
		Keys.ChronoKillCargo = ini.Get_Bool(IniName, "Chronosphere.KillCargo", Keys.ChronoKillCargo);
		Keys.ChronoReconsiderBuildings = ini.Get_Bool(IniName, "Chronosphere.ReconsiderBuildings", Keys.ChronoReconsiderBuildings);
		Keys.ChronoAffectsUndeployable = ini.Get_Bool(IniName, "Chronosphere.AffectsUndeployable", Keys.ChronoAffectsUndeployable);
		Keys.ChronoBlowUnplaceable = ini.Get_Bool(IniName, "Chronosphere.BlowUnplaceable", Keys.ChronoBlowUnplaceable);
		Keys.PostDependent = SuperKeyParse::Text_Or(ini.Get_String(IniName, "SW.PostDependent"), Keys.PostDependent);

		if (ini.Is_Present(IniName, "Mutate.Explosion")) {
			Keys.MutateExplosion = ini.Get_Bool(IniName, "Mutate.Explosion", false) ? 1 : 0;
		}
		Keys.MutateIgnoreCyborg = ini.Get_Bool(IniName, "Mutate.IgnoreCyborg", Keys.MutateIgnoreCyborg);
		Keys.MutateIgnoreNotHuman = ini.Get_Bool(IniName, "Mutate.IgnoreNotHuman", Keys.MutateIgnoreNotHuman);
		Keys.MutateKillNatural = ini.Get_Bool(IniName, "Mutate.KillNatural", Keys.MutateKillNatural);
		Keys.LightningIgnoreRod = ini.Get_Bool(IniName, "Lightning.IgnoreLightningRod", Keys.LightningIgnoreRod);
		Keys.VirtualCharge = ini.Get_Bool(IniName, "SW.VirtualCharge", Keys.VirtualCharge);

		Keys.SonarPulseDelay = ini.Get_Int(IniName, "SonarPulse.Delay", Keys.SonarPulseDelay);

		Keys.BatteryPower = ini.Get_Int(IniName, "Battery.Power", Keys.BatteryPower);
		SuperKeyParse::Building_List(ini.Get_String(IniName, "Battery.Overpower"), Keys.BatteryOverpower);
		SuperKeyParse::Building_List(ini.Get_String(IniName, "Battery.KeepOnline"), Keys.BatteryKeepOnline);

		Keys.SpyPlaneType = SuperKeyParse::Aircraft_Named(ini.Get_String(IniName, "SpyPlane.Type"), Keys.SpyPlaneType);
		Keys.SpyPlaneCount = ini.Get_Int(IniName, "SpyPlane.Count", Keys.SpyPlaneCount);
		{
			std::string const name = ini.Get_String(IniName, "SpyPlane.Mission");
			MissionType const mission = name.empty() ? MISSION_NONE : MissionClass::Mission_From_Name(name.c_str());
			if (mission != MISSION_NONE) {
				Keys.SpyPlaneMission = mission;
			}
		}

		Keys.ParaDropCount = ini.Get_Int(IniName, "ParaDrop.Count", Keys.ParaDropCount);
		SuperKeyParse::Techno_List(ini.Get_String(IniName, "ParaDrop.Types"), Keys.ParaDefault.Types);
		Keys.ParaDefault.Num = ini.Get_IntList(IniName, "ParaDrop.Num", Keys.ParaDefault.Num);
		Keys.ParaDefault.Aircraft = SuperKeyParse::Aircraft_Named(ini.Get_String(IniName, "ParaDrop.Aircraft"), Keys.ParaDefault.Aircraft);
		Keys.ParaDefault.IsSet = Keys.ParaDefault.Types.Count() > 0 || Keys.ParaDefault.Num.Count() > 0 || Keys.ParaDefault.Aircraft != NULL;
		Keys.Read_Para_Owners(ini, IniName);

		// Ares forces the charge drain on for the behaviors that run on it.
		if (Type == SUPER_BATTERY || Type == SUPER_FIRESTORM) {
			UseChargeDrain = true;
		}
		if (Type == SUPER_BATTERY) {
			BatteryTypesExist = true;
		}

		float recharge = ini.Get_Float(IniName, "RechargeTime");
		if (recharge != 0.0) {
			RechargeTime = recharge * TICKS_PER_MINUTE;
		}

		ini.Get_String(IniName, "SidebarImage", SidebarImage);
		char pcx[64];
		ini.Get_String(IniName, "SidebarPCX", SidebarPCX.c_str(), pcx, sizeof(pcx));
		SidebarPCX = pcx;

		char fullname[_MAX_FNAME+_MAX_EXT];
		_makepath(fullname, NULL, NULL, SidebarImage, ".SHP");
		CameoData = (ShapeSet *)MFCD::Retrieve(fullname);
		if (CameoData == NULL) {
			CameoData = (ShapeSet *)MFCD::Retrieve("XXICON.SHP");
		}

		return(true);
	}
	return(false);
}


/// <summary>
/// Converts a rules name into a super weapon type number.
/// </summary>
/// <returns>Returns with the super weapon type that bears the name. Otherwise, SUPER_NONE
/// is returned.</returns>
SuperWeaponType SuperWeaponTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int classid = SUPER_FIRST; classid < SuperWeaponTypes.Count(); classid++) {
			if (stricmp(SuperWeaponTypes[classid]->IniName, name) == 0) {
				return(SuperWeaponType)classid;
			}
		}
	}
	return(SUPER_NONE);
}


/// <summary>
/// Fetches the super weapon type that performs the action specified.
/// This routine is used to work back from a mouse action to the super weapon that
/// offers it.
/// </summary>
/// <returns>Returns with a pointer to the matching super weapon type. Otherwise, NULL is
/// returned.</returns>
SuperWeaponTypeClass * SuperWeaponTypeClass::From_Action(ActionType action)
{
	for (int classid = SUPER_FIRST; classid < SuperWeaponTypes.Count(); classid++) {
		if (SuperWeaponTypes[classid]->Action == action) {
			return(SuperWeaponTypes[classid]);
		}
	}
	return(NULL);
}


/// <summary>
/// Fetches the super weapon type of the name specified, creating it if need be.
/// This routine is used while the rules are being parsed so that a super weapon may be
/// referred to before its own section has been reached.
/// </summary>
/// <returns>Returns with a pointer to the super weapon type object.</returns>
SuperWeaponTypeClass * SuperWeaponTypeClass::Find_Or_Make(const char *name)
{
	return(TFind_Or_Make<SuperWeaponTypeClass>(name, SuperWeaponTypes));
}


/// <summary>
/// Determines the action to perform when this super weapon is aimed at a target.
/// Most super weapons simply report the action their rules entry names. The EM pulse is
/// the exception, since it must be delivered by a powered pulse cannon -- this routine
/// tells the cursor logic whether the player has one that can reach the target.
/// </summary>
/// <param name="object">The object being targeted, or NULL if the cell itself is the
/// target.</param>
/// <returns>Returns with the action to perform, which the caller turns into a mouse
/// shape.</returns>
ActionType SuperWeaponTypeClass::Stock_Action(Cell const & cell, ObjectClass * object)
{
	// A force shield offers its own cursor only over a structure of the player or an ally; elsewhere
	// it is the no-shield cursor (SuperWeaponTypeClass::MouseOverObject, 0x6CEF80).
	if (Type == SUPER_FORCE_SHIELD) {
		if (object == NULL || object->RTTI != RTTI_BUILDING || !((BuildingClass *)object)->House->Is_Ally(PlayerPtr)) {
			return(Action == ACTION_CUSTOM ? ACTION_NO_CUSTOM : ACTION_NO_FORCE_SHIELD);
		}
		return(Action);
	}

	if (Type != SUPER_EM_PULSE) {
		return(Action);
	}

	AbstractClass * target;
	Cell target_cell;

	if (object != NULL) {
		target = object;
		target_cell = object->PositionCell;
	} else {
		target = &Map[cell];
		target_cell = cell;
	}

	BuildingClass * cannon = NULL;
	int mindist = INT_MAX;

	for (int i = 0; i < Buildings.Count(); i++) {
		BuildingClass * building = Buildings[i];
		if (building->Class->IsEMPulseCannon && building->House == PlayerPtr && building->Is_Powered_On() && Keys.Allows_Cannon(building->Class)) {
			if (building->Distance(target) < mindist) {
				mindist = building->Distance(target);
				cannon = building;
			}
		}
	}

	// A weapon with Action=Custom keeps that action in range, and takes NoCustom out of range.
	ActionType const in_range = Action == ACTION_CUSTOM ? ACTION_CUSTOM : ACTION_EMPULSE;
	ActionType const out_of_range = Action == ACTION_CUSTOM ? ACTION_NO_CUSTOM : ACTION_EMPULSE_RANGE;

	if (cannon == NULL) {
		return(out_of_range);
	}

	auto const * data = cannon->Get_Class_Weapon_Data();
	WeaponTypeClass * weap = data != NULL ? data->Weapon : NULL;
	if (Keys.RangeMaximum < 0.0f && weap == NULL) {
		return(out_of_range);
	}
	int range = Keys.RangeMaximum >= 0.0f ? (int)Keys.RangeMaximum : weap->Range / CELL_LEPTON;
	Cell cannon_cell = cannon->PositionCell;
	Cell dist(target_cell - cannon_cell);
	if (dist.X * dist.X + dist.Y * dist.Y < range * range) {
		return(in_range);
	}

	return(out_of_range);
}
