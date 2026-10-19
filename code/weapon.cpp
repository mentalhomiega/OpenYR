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

/* $Header: /CounterStrike/WEAPON.CPP 1     3/03/97 10:26a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : WEAPON.CPP                                                   *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : 05/20/96                                                     *
 *                                                                                             *
 *                  Last Update : September 9, 1996 [JLB]                                      *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   Armor_From_Name -- Convert ASCII name into armor type number.                             *
 *   WeaponTypeClass::As_Pointer -- Give a weapon type ID, fetch pointer to weapon type object.*
 *   WeaponTypeClass::Read_INI -- Fetch the weapon data from the INI database.                 *
 *   WeaponTypeClass::WeaponTypeClass -- Default constructor for weapon type objects.          *
 *   WeaponTypeClass::operator delete -- Returns weapon type object back to special heap.      *
 *   WeaponTypeClass::operator new -- Allocates a weapon type object form the special heap.    *
 *   WeaponTypeClass::~WeaponTypeClass -- Destructor for weapon type class objects.            *
 *   Weapon_From_Name -- Conver ASCII name to weapon type ID number.                           *
 *   WeaponTypeClass::Allowed_Threats -- Determine what threats this weapon can address.       *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "armortypes.h"
#include "weapon.h"

#include "_rules.h"
#include "_weapon.h"
#include "animtype.h"
#include "bullettype.h"
#include "combat.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "psystype.h"
#include "rules.h"
#include "savestream.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "emissive.h"
#include "objectkeyrules.h"
#include "rtti.hh"
#include "warhead.h"

#include <algorithm>
#include <cstring>


/***********************************************************************************************
 * WeaponTypeClass::WeaponTypeClass -- Default constructor for weapon type objects.            *
 *                                                                                             *
 *    This default constructor will initialize all the values of the weapon to the default     *
 *    state. Thus, if any of these settings are not specifically overridden by the rules.ini   *
 *    file, they will remain this value in the game.                                           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/17/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
WeaponTypeClass::WeaponTypeClass(char const * ininame) :
	BASECLASS(ininame),
	IsSupressed(false),
	IsCamera(false),
	IsInfiniteMindControl(false),
	IsFireInTransport(true),
	IsFireWhileMoving(true),
	IsRadBeam(false),
	IsRadEruption(false),
	RadLevel(0),
	IsLimboLaunch(false),
	IsFireOnce(false),
	IsAbductor(false),
	AbductBelowPercent(1.0),
	AbductAnim(NULL),
	IsAbductChangeOwner(false),
	HasIvanDelay(false),
	IvanDelay(0),
	IvanWarhead(NULL),
	HasIvanDamage(false),
	IvanDamage(0),
	IvanAttachSound(IVAN_SOUND_UNSET),
	IvanTickingSound(IVAN_SOUND_UNSET),
	IvanImage(),
	IvanDetachable(-1),
	IvanDetonateOnSell(-1),
	IvanDestroysBridges(-1),
	IvanCanDetonateTimeBomb(-1),
	IvanImageData(NULL),
	IvanImageLooked(false),
	OpenToppedAnim(NULL),
	OccupantAnim(NULL),
	AssaultAnim(NULL),
	IsElectric(false),
	IsLaser(false),
	IsMigAttackCursor(false),
	IsSabotageCursor(false),
	IsElectricBolt(false),
	EmissiveStrength(0),
	IsSpawner(false),
	IsSuicide(false),
	IsDiskLaser(false),
	IsDrainWeapon(false),
	IsAlternateColor(false),
	IsIonSensitive(false),
	Burst(1),
	Bullet(NULL),
	Attack(0),
	MaxSpeed(MPH_IMMOBILE),
	WarheadPtr(NULL),
	ROF(0),
	Range(0),
	AmbientDamage(0),
	ProjectileRange(100000),
	MinimumRange(0),
	AttachedParticleSystem(NULL),
	LaserInnerColor(0, 0, 0),
	LaserOuterColor(0, 0, 0),
	LaserOuterSpread(0, 0, 0),
	UseFireParticles(false),
	UseSparkParticles(false),
	IsRailgun(false),
	IsDetachedRailgun(false),
	BoltColorMask(0),
	HasBeamColor(false),
	BeamDuration(-1),
	BeamAmplitude(-1.0),
	LaserThickness(1),
	HasWaveColor(false),
	HasWaveIntensity(false),
	WaveReverseVehicles(-1),
	WaveReverseBuildings(-1),
	WaveReverseInfantry(-1),
	WaveReverseAircraft(-1),
	WaveReverseOthers(-1),
	IsLobber(false),
	IsBright(false),
	LaserDuration(10),
	IsBigLaser(false),
	IsSonic(false),
	IsMagBeam(false),
	IsDecloakToFire(true),
	IsAreaFire(false),
	IsNeverUse(false),
	IsCellRangefinding(false),
	IsOmniFire(false),
	IsRevealOnFire(true),
	IsTerrainFire(false),
	DisguiseFakeBlinkTime(0),
	IsHouseColor(false),
	IsTurboBoosted(false),
	Sound(),
	Anim()
{
	for (int i = 0; i < ARRAY_SIZE(BurstDelay); i++) {
		BurstDelay[i] = -1;
	}

	Create_ID();
	Weapons.Add(this);
}


/***********************************************************************************************
 * WeaponTypeClass::~WeaponTypeClass -- Destructor for weapon type class objects.              *
 *                                                                                             *
 *    This destructor really doesn't do anything but set the pointers to NULL. This is a       *
 *    general purposes safety tactic but is otherwise useless.                                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/17/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
WeaponTypeClass::~WeaponTypeClass(void)
{
	Detach_This_From_All(this, true);
	Bullet = NULL;
	WarheadPtr = NULL;

	Weapons.Delete(this);
}


/***********************************************************************************************
 * WeaponTypeClass::Read_INI -- Fetch the weapon data from the INI database.                   *
 *                                                                                             *
 *    This routine will fetch the weapon data for this weapon type object from the INI         *
 *    database specified.                                                                      *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the INI database that the weapon data will be fetched        *
 *                   from.                                                                     *
 *                                                                                             *
 * OUTPUT:  bool; Was this weapon type described in the database and the values retrieved?     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/19/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool WeaponTypeClass::Read_INI(CCINIClass const & ini)
{
	if (ini.Is_Present(IniName)) {
		AmbientDamage = ini.Get_Int(IniName, "AmbientDamage", AmbientDamage);
		IsSonic = ini.Get_Bool(IniName, "IsSonic", IsSonic);
		IsMagBeam = ini.Get_Bool(IniName, "IsMagBeam", IsMagBeam);
		IsDecloakToFire = ini.Get_Bool(IniName, "DecloakToFire", IsDecloakToFire);
		IsAreaFire = ini.Get_Bool(IniName, "AreaFire", IsAreaFire);
		IsNeverUse = ini.Get_Bool(IniName, "NeverUse", IsNeverUse);
		IsCellRangefinding = ini.Get_Bool(IniName, "CellRangefinding", IsCellRangefinding);
		IsOmniFire = ini.Get_Bool(IniName, "OmniFire", IsOmniFire);
		IsRevealOnFire = ini.Get_Bool(IniName, "RevealOnFire", IsRevealOnFire);
		IsTerrainFire = ini.Get_Bool(IniName, "TerrainFire", IsTerrainFire);
		DisguiseFakeBlinkTime = ini.Get_Int(IniName, "DisguiseFakeBlinkTime", DisguiseFakeBlinkTime);
		IsHouseColor = ini.Get_Bool(IniName, "IsHouseColor", IsHouseColor);
		IsSupressed = ini.Get_Bool(IniName, "Supress", IsSupressed);
		Burst = ini.Get_Int(IniName, "Burst", Burst);
		if (Burst < 1) {
			Burst = 1;
		}
		Attack = ini.Get_Int(IniName, "Damage", Attack);
		MaxSpeed = ini.Get_MPHType(IniName, "Speed", MaxSpeed);
		ROF = ini.Get_Int(IniName, "ROF", ROF);
		Range = ini.Get_Lepton(IniName, "Range", Range);
		ProjectileRange = ini.Get_Lepton(IniName, "ProjectileRange", ProjectileRange);

		for (int i = 0; i < ARRAY_SIZE(BurstDelay); i++) {
			char buf[20];
			sprintf(buf, "BurstDelay%d", i);
			BurstDelay[i] = ini.Get_Int(IniName, buf, BurstDelay[i]);
		}

		MinimumRange = ini.Get_Lepton(IniName, "MinimumRange", MinimumRange);

		Sound = ini.Get_VocType_List(ini, IniName, "Report", Sound);
		Anim = TGet_TypeList<AnimTypeClass>(ini, IniName, "Anim", Anim);

		IsCamera = ini.Get_Bool(IniName, "Camera", IsCamera);
		IsInfiniteMindControl = ini.Get_Bool(IniName, "InfiniteMindControl", IsInfiniteMindControl);
		IsFireInTransport = ini.Get_Bool(IniName, "FireInTransport", IsFireInTransport);
		IsFireWhileMoving = ini.Get_Bool(IniName, "FireWhileMoving", IsFireWhileMoving);
		IsRadBeam = ini.Get_Bool(IniName, "IsRadBeam", IsRadBeam);
		IsRadEruption = ini.Get_Bool(IniName, "IsRadEruption", IsRadEruption);
		RadLevel = ini.Get_Int(IniName, "RadLevel", RadLevel);
		IsLimboLaunch = ini.Get_Bool(IniName, "LimboLaunch", IsLimboLaunch);
		IsFireOnce = ini.Get_Bool(IniName, "FireOnce", IsFireOnce);
		IsAbductor = ini.Get_Bool(IniName, "Abductor", IsAbductor);
		AbductBelowPercent = ini.Get_Float(IniName, "Abductor.AbductBelowPercent", AbductBelowPercent);
		AbductAnim = TGet_Class(ini, IniName, "Abductor.Anim", AbductAnim);
		IsAbductChangeOwner = ini.Get_Bool(IniName, "Abductor.ChangeOwner", IsAbductChangeOwner);
		if (ini.Is_Present(IniName, "IvanBomb.Delay")) {
			IvanDelay = ini.Get_Int(IniName, "IvanBomb.Delay", IvanDelay);
			HasIvanDelay = true;
		}
		IvanWarhead = TGet_Class(ini, IniName, "IvanBomb.Warhead", IvanWarhead);
		if (ini.Is_Present(IniName, "IvanBomb.Damage")) {
			IvanDamage = ini.Get_Int(IniName, "IvanBomb.Damage", IvanDamage);
			HasIvanDamage = true;
		}
		if (ini.Is_Present(IniName, "IvanBomb.AttachSound")) {
			IvanAttachSound = ini.Get_VocType(IniName, "IvanBomb.AttachSound", VOC_NONE);
		}
		if (ini.Is_Present(IniName, "IvanBomb.TickingSound")) {
			IvanTickingSound = ini.Get_VocType(IniName, "IvanBomb.TickingSound", VOC_NONE);
		}
		if (ini.Is_Present(IniName, "IvanBomb.Image")) {
			IvanImage = ini.Get_String(IniName, "IvanBomb.Image", "");
			IvanImageData = NULL;
			IvanImageLooked = false;
		}
		if (ini.Is_Present(IniName, "IvanBomb.Detachable")) {
			IvanDetachable = ini.Get_Bool(IniName, "IvanBomb.Detachable", true) ? 1 : 0;
		}
		if (ini.Is_Present(IniName, "IvanBomb.DetonateOnSell")) {
			IvanDetonateOnSell = ini.Get_Bool(IniName, "IvanBomb.DetonateOnSell", true) ? 1 : 0;
		}
		if (ini.Is_Present(IniName, "IvanBomb.DestroysBridges")) {
			IvanDestroysBridges = ini.Get_Bool(IniName, "IvanBomb.DestroysBridges", true) ? 1 : 0;
		}
		if (ini.Is_Present(IniName, "IvanBomb.CanDetonateTimeBomb")) {
			IvanCanDetonateTimeBomb = ini.Get_Bool(IniName, "IvanBomb.CanDetonateTimeBomb", false) ? 1 : 0;
		}
		OpenToppedAnim = TGet_Class(ini, IniName, "OpenToppedAnim", OpenToppedAnim);
		OccupantAnim = TGet_Class(ini, IniName, "OccupantAnim", OccupantAnim);
		AssaultAnim = TGet_Class(ini, IniName, "AssaultAnim", AssaultAnim);
		IsLaser = ini.Get_Bool(IniName, "IsLaser", IsLaser);
		IsMigAttackCursor = ini.Get_Bool(IniName, "MigAttackCursor", IsMigAttackCursor);
		IsSabotageCursor = ini.Get_Bool(IniName, "SabotageCursor", IsSabotageCursor);
		IsElectricBolt = ini.Get_Bool(IniName, "IsElectricBolt", IsElectricBolt);
		EmissiveStrength = Emissive_Parse_Strength(ini.Get_String(IniName, "Emissive", "").c_str(), IsLaser ? EmissiveDefaults[EMISSIVE_LASER].Strength : (IsElectricBolt ? EmissiveDefaults[EMISSIVE_ELECTRIC_BOLT].Strength : 0));
		IsSpawner = ini.Get_Bool(IniName, "Spawner", IsSpawner);
		IsSuicide = ini.Get_Bool(IniName, "Suicide", IsSuicide);
		IsDiskLaser = ini.Get_Bool(IniName, "DiskLaser", IsDiskLaser);
		IsDrainWeapon = ini.Get_Bool(IniName, "DrainWeapon", IsDrainWeapon);
		IsAlternateColor = ini.Get_Bool(IniName, "IsAlternateColor", IsAlternateColor);
		IsElectric = ini.Get_Bool(IniName, "Charges", IsElectric);
		IsTurboBoosted = ini.Get_Bool(IniName, "TurboBoost", IsTurboBoosted);

		UseFireParticles = ini.Get_Bool(IniName, "UseFireParticles", UseFireParticles);
		UseSparkParticles = ini.Get_Bool(IniName, "UseSparkParticles", UseSparkParticles);
		IsRailgun = ini.Get_Bool(IniName, "IsRailgun", IsRailgun);
		IsDetachedRailgun = ini.Get_Bool(IniName, "IsDetachedRailgun", IsDetachedRailgun);
		if (ini.Is_Present(IniName, "Bolt.Color1")) {
			BoltColor[0] = ini.Get_RGBClass(IniName, "Bolt.Color1", BoltColor[0]);
			BoltColorMask |= 1;
		}
		if (ini.Is_Present(IniName, "Bolt.Color2")) {
			BoltColor[1] = ini.Get_RGBClass(IniName, "Bolt.Color2", BoltColor[1]);
			BoltColorMask |= 2;
		}
		if (ini.Is_Present(IniName, "Bolt.Color3")) {
			BoltColor[2] = ini.Get_RGBClass(IniName, "Bolt.Color3", BoltColor[2]);
			BoltColorMask |= 4;
		}
		if (ini.Is_Present(IniName, "Beam.Color")) {
			BeamColor = ini.Get_RGBClass(IniName, "Beam.Color", BeamColor);
			HasBeamColor = true;
		}
		IsLobber = ini.Get_Bool(IniName, "Lobber", IsLobber);

		LaserInnerColor = ini.Get_RGBClass(IniName, "LaserInnerColor", LaserInnerColor);
		LaserOuterColor = ini.Get_RGBClass(IniName, "LaserOuterColor", LaserOuterColor);
		LaserOuterSpread = ini.Get_RGBClass(IniName, "LaserOuterSpread", LaserOuterSpread);
		LaserDuration = ini.Get_Int(IniName, "LaserDuration", LaserDuration);
		IsBigLaser = ini.Get_Bool(IniName, "IsBigLaser", IsBigLaser);
		IsBigLaser = ini.Get_Bool(IniName, "Wave.IsBigLaser", IsBigLaser);
		LaserThickness = std::max(1, ini.Get_Int(IniName, "LaserThickness", LaserThickness));
		BeamDuration = ini.Get_Int(IniName, "Beam.Duration", BeamDuration);
		BeamAmplitude = ini.Get_Float(IniName, "Beam.Amplitude", BeamAmplitude);
		if (ini.Is_Present(IniName, "Wave.Color")) {
			HasWaveColor = ObjectKeyRules::Parse_Triplet(ini.Get_String(IniName, "Wave.Color"), WaveColor);
		}
		if (ini.Is_Present(IniName, "Wave.Intensity")) {
			HasWaveIntensity = ObjectKeyRules::Parse_Triplet(ini.Get_String(IniName, "Wave.Intensity"), WaveIntensity);
		}
		if (ini.Is_Present(IniName, "Wave.ReverseAgainstVehicles")) WaveReverseVehicles = ini.Get_Bool(IniName, "Wave.ReverseAgainstVehicles", false) ? 1 : 0;
		if (ini.Is_Present(IniName, "Wave.ReverseAgainstBuildings")) WaveReverseBuildings = ini.Get_Bool(IniName, "Wave.ReverseAgainstBuildings", false) ? 1 : 0;
		if (ini.Is_Present(IniName, "Wave.ReverseAgainstInfantry")) WaveReverseInfantry = ini.Get_Bool(IniName, "Wave.ReverseAgainstInfantry", false) ? 1 : 0;
		if (ini.Is_Present(IniName, "Wave.ReverseAgainstAircraft")) WaveReverseAircraft = ini.Get_Bool(IniName, "Wave.ReverseAgainstAircraft", false) ? 1 : 0;
		if (ini.Is_Present(IniName, "Wave.ReverseAgainstOthers")) WaveReverseOthers = ini.Get_Bool(IniName, "Wave.ReverseAgainstOthers", false) ? 1 : 0;
		if (ini.Is_Present(IniName, "Cursor.Attack")) CursorAttack = ini.Get_String(IniName, "Cursor.Attack");
		if (ini.Is_Present(IniName, "Cursor.AttackOutOfRange")) CursorAttackOutOfRange = ini.Get_String(IniName, "Cursor.AttackOutOfRange");

		IsBright = ini.Get_Bool(IniName, "Bright", IsBright);
		IsIonSensitive = ini.Get_Bool(IniName, "IonSensitive", IsIonSensitive);

		char buffer[20];
		buffer[0] = '\0';
		ini.Get_String(IniName, "AttachedParticleSystem", buffer, buffer, sizeof(buffer));
		ParticleSystemType ps = ParticleSystemTypeClass::From_Name(buffer);
		if (ps != PARTSYS_NONE) {
			AttachedParticleSystem = ParticleSystemTypes[ps];
		}

		WarheadPtr = TGet_Class(ini, IniName, "Warhead", WarheadPtr);
		Bullet = TGet_Class(ini, IniName, "Projectile", Bullet);

		return(true);
	}
	return(false);
}


/// <summary>
/// Sets the launch speed for a ballistic weapon.
/// A weapon that throws an unguided projectile must launch it hard enough to carry it the
/// weapon's full range, so the speed is worked out from that range and the gravity the
/// projectile falls under. A guided projectile steers itself and is left alone.
/// </summary>
/// <remarks>Only call this routine once the rules have been read, since it leans on them.</remarks>
void WeaponTypeClass::Init_Max_Speed(void)
{
	if (Bullet != NULL && Bullet->ROT == 0) {
		double gravity = Rule->Gravity;
		if (Bullet->IsFloater) {
			gravity = Get_Floater_Gravity();
		}
		MaxSpeed = Calculate_Projectile_Speed(Range, gravity);
	}
}


/***********************************************************************************************
 * Armor_From_Name -- Convert ASCII name into armor type number.                               *
 *                                                                                             *
 *    This will find the armor type that matches the ASCII name specified and then will return *
 *    the armor ID number of it.                                                               *
 *                                                                                             *
 * INPUT:   name  -- Pointer to the ASCII name of the armor to find.                           *
 *                                                                                             *
 * OUTPUT:  Returns with the armor ID number of the armor that matches the name specified. If  *
 *          no match could be found, then ARMOR_NONE is returned.                              *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/17/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
ArmorType Armor_From_Name(char const * name)
{
	if (!name) return(ARMOR_NONE);

	for (int index = ARMOR_FIRST; index < Armor_Type_Count(); index++) {
		if (stricmp(Armor_Type_Name(ArmorType(index)), name) == 0) {
			return(ArmorType(index));
		}
	}

	return(ARMOR_NONE);
}


/***********************************************************************************************
 * WeaponTypeClass::Allowed_Threats -- Determine what threats this weapon can address.         *
 *                                                                                             *
 *    This routine will examine the capabilities of this weapon and return with the threat     *
 *    types that it can address.                                                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the threat types that this weapon can address.                        *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/09/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
ThreatType WeaponTypeClass::Allowed_Threats(void) const
{
	if (Bullet->IsAntiVehicle) {
		return(THREAT_VEHICLES);
	}
	ThreatType threat = THREAT_NORMAL;
	if (Bullet->IsAntiAircraft) {
		threat = ThreatType(threat | THREAT_AIR);
	}
	if (Bullet->IsAntiGround) {
		threat = ThreatType(threat | THREAT_INFANTRY|THREAT_VEHICLES|THREAT_BOATS|THREAT_BUILDINGS);
	}
	return(threat);
}


/// <summary>
/// Can this weapon knock down a wall?
/// The answer comes from the warhead the weapon fires, and it is used when deciding whether
/// an object should bother attacking a wall that is in its way.
/// </summary>
/// <returns>bool; Will this weapon destroy walls?</returns>
bool WeaponTypeClass::Is_Wall_Destroyer(void) const
{
	if (WarheadPtr != NULL && WarheadPtr->IsWallDestroyer) {
		return(true);
	}
	return(false);
}


/// <summary>
/// Adds this weapon type's data to the CRC calculation.
/// This routine is used by the multiplayer sync checker to prove that every machine agrees
/// about the weapon, so the projectile and warhead it refers to are submitted by identifier
/// rather than by pointer.
/// </summary>
/// <param name="crc">The checksum engine to submit the data to.</param>
void WeaponTypeClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(AmbientDamage);
	crc(IsSonic);
	crc(IsCellRangefinding);
	crc(IsOmniFire);
	crc(IsRevealOnFire);
	crc(IsTerrainFire);
	crc(DisguiseFakeBlinkTime);
	crc(IsFireWhileMoving);
	if (AssaultAnim != NULL) crc(AssaultAnim->Fetch_ID());
	crc(IsHouseColor);
	crc(IsTurboBoosted);
	crc(IsSupressed);
	crc(IsCamera);
	crc(IsInfiniteMindControl);
	crc(IsFireInTransport);
	crc(IsRadBeam);
	crc(IsRadEruption);
	crc(RadLevel);
	crc(IsLimboLaunch);
	crc(IsFireOnce);
	crc(IsLaser);
	crc(IsMigAttackCursor);
	crc(IsSabotageCursor);
	crc(IsElectricBolt);
	crc(IsAlternateColor);
	crc(Burst);
	if (Bullet != NULL) crc(Bullet->Fetch_ID());
	crc(Attack);
	crc(MaxSpeed);
	if (WarheadPtr != NULL) crc(WarheadPtr->Fetch_ID());
	crc(ROF);
	crc(Range);
	crc(Sound.Count());
	crc(Anim.Count());
	crc(UseFireParticles);
	crc(IsLobber);
	crc((char)LaserDuration);
	crc(IsBigLaser);
	crc(IsRailgun);
	crc(IsDetachedRailgun);
	crc(IsElectric);
	crc(IsBright);
	crc(UseSparkParticles);
}


ClassID WeaponTypeClass::Class_ID(void) const
{
	return(ClassID_WeaponTypeClass);
}


/// <summary>
/// Lists the members this weapon type carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void WeaponTypeClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(AmbientDamage);
	stream.Serialize(Burst);
	stream.Serialize(Bullet);
	stream.Serialize(Attack);
	stream.Serialize(MaxSpeed);
	stream.Serialize(WarheadPtr);
	stream.Serialize(ROF);
	stream.Serialize(Range);
	stream.Serialize(ProjectileRange);
	stream.Serialize(BurstDelay);
	stream.Serialize(MinimumRange);
	stream.Serialize(Sound);
	stream.Serialize(Anim);
	stream.Serialize(AttachedParticleSystem);
	stream.Serialize(LaserInnerColor);
	stream.Serialize(LaserOuterColor);
	stream.Serialize(LaserOuterSpread);
	stream.Serialize(UseFireParticles);
	stream.Serialize(UseSparkParticles);
	stream.Serialize(IsRailgun);
	stream.Serialize(IsLobber);
	stream.Serialize(IsBright);
	stream.Serialize(LaserDuration);
	stream.Serialize(IsBigLaser);
	stream.Serialize(IsSonic);
	stream.Serialize(IsMagBeam);
	stream.Serialize(IsDecloakToFire);
	stream.Serialize(IsAreaFire);
	stream.Serialize(IsNeverUse);
	stream.Serialize(IsCellRangefinding);
	stream.Serialize(IsOmniFire);
	stream.Serialize(IsRevealOnFire);
	stream.Serialize(IsTerrainFire);
	stream.Serialize(DisguiseFakeBlinkTime);
	stream.Serialize(IsFireWhileMoving);
	stream.Serialize(IsHouseColor);
	stream.Serialize(IsTurboBoosted);
	stream.Serialize(IsSupressed);
	stream.Serialize(IsCamera);
	stream.Serialize(IsInfiniteMindControl);
	stream.Serialize(IsFireInTransport);
	stream.Serialize(IsRadBeam);
	stream.Serialize(IsRadEruption);
	stream.Serialize(RadLevel);
	stream.Serialize(IsLimboLaunch);
	stream.Serialize(IsFireOnce);
	stream.Serialize(OpenToppedAnim);
	stream.Serialize(OccupantAnim);
	stream.Serialize(AssaultAnim);
	stream.Serialize(IsElectric);
	stream.Serialize(IsLaser);
	stream.Serialize(IsMigAttackCursor);
	stream.Serialize(IsSabotageCursor);
	stream.Serialize(IsElectricBolt);
	stream.Serialize(IsSpawner);
	stream.Serialize(IsSuicide);
	stream.Serialize(IsDiskLaser);
	stream.Serialize(IsDrainWeapon);
	stream.Serialize(IsAlternateColor);
	stream.Serialize(IsIonSensitive);
	stream.Serialize(IsAbductor);
	stream.Serialize(AbductBelowPercent);
	stream.Serialize(AbductAnim);
	stream.Serialize(IsAbductChangeOwner);
	stream.Serialize(HasIvanDelay);
	stream.Serialize(IvanDelay);
	stream.Serialize(IvanWarhead);
	stream.Serialize(HasIvanDamage);
	stream.Serialize(IvanDamage);
	stream.Serialize(IvanAttachSound);
	stream.Serialize(IvanTickingSound);
	stream.Serialize(IvanDetachable);
	stream.Serialize(IvanDetonateOnSell);
	stream.Serialize(IvanDestroysBridges);
	stream.Serialize(IvanCanDetonateTimeBomb);
	stream.Serialize(IsDetachedRailgun);
	stream.Serialize(BoltColorMask);
	stream.Serialize(BoltColor[0]);
	stream.Serialize(BoltColor[1]);
	stream.Serialize(BoltColor[2]);
	stream.Serialize(HasBeamColor);
	stream.Serialize(BeamColor);
	stream.Serialize(BeamDuration);
	stream.Serialize(BeamAmplitude);
	stream.Serialize(LaserThickness);
	stream.Serialize(HasWaveColor);
	stream.Serialize(WaveColor);
	stream.Serialize(HasWaveIntensity);
	stream.Serialize(WaveIntensity);
	stream.Serialize(WaveReverseVehicles);
	stream.Serialize(WaveReverseBuildings);
	stream.Serialize(WaveReverseInfantry);
	stream.Serialize(WaveReverseAircraft);
	stream.Serialize(WaveReverseOthers);
	stream.Serialize(CursorAttack);
	stream.Serialize(CursorAttackOutOfRange);
}


/// <summary>
/// Is a wave this weapon draws between the firer and a target of this kind drawn from the target back to the firer? A
/// weapon that sets no Wave.ReverseAgainst key for the kind does so only against a vehicle, and only when it is a magnetron.
/// </summary>
bool WeaponTypeClass::Wave_Is_Reversed_Against(int rtti) const
{
	int setting = -1;
	bool by_default = false;
	switch (rtti) {
		case RTTI_UNIT:
			setting = WaveReverseVehicles;
			by_default = IsMagBeam;
			break;
		case RTTI_BUILDING:
			setting = WaveReverseBuildings;
			break;
		case RTTI_INFANTRY:
			setting = WaveReverseInfantry;
			break;
		case RTTI_AIRCRAFT:
			setting = WaveReverseAircraft;
			break;
		default:
			setting = WaveReverseOthers;
			break;
	}
	return(setting < 0 ? by_default : setting != 0);
}


/// <summary>
/// Fetches the weapon of the name specified, creating it if it does not exist yet.
/// Use this routine while processing the rules so that a weapon mentioned before its own
/// section has been read will still resolve to a real object.
/// </summary>
/// <param name="name">The INI name of the weapon desired.</param>
/// <returns>Returns with a pointer to the weapon type of that name.</returns>
WeaponTypeClass * WeaponTypeClass::Find_Or_Make(const char *name)
{
	return(TFind_Or_Make<WeaponTypeClass>(name, Weapons));
}


/***********************************************************************************************
 * Weapon_From_Name -- Conver ASCII name to weapon type ID number.                             *
 *                                                                                             *
 *    This will find the weapon whos name matches that specified and then it will return the   *
 *    weapon ID number associated with it.                                                     *
 *                                                                                             *
 * INPUT:   name  -- Pointer to the ASCII name of the weapon type.                             *
 *                                                                                             *
 * OUTPUT:  Returns with the weapon type ID number that matches the name specified. If no      *
 *          match could be found, then WEAPON_NONE is returned.                                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/17/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
WeaponType WeaponTypeClass::From_Name(char const * name)
{
	for (int index = 0; index < Weapons.Count(); index++) {
		if (stricmp(Weapons[index]->IniName, name) == 0) {
			return(WeaponType(index));
		}
	}
	return(WEAPON_NONE);
}
