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

#include "always.h"

#include "techtype.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "airctype.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "classids.h"
#include "combat.h"
#include "findmake.h"
#include "globals.h"
#include "infatype.h"
#include "mixfile.h"
#include "psystype.h"
#include "rules.h"
#include "savestream.h"
#include "scenario.h"
#include "session.h"
#include "tracker.h"
#include "unittype.h"
#include "vanimtype.h"
#include "weapon.h"

#include "pip.hh"
#include "voc.hh"

#include <algorithm>
#include <new.h>


/***************************************************************************
**	These are the pointers to the special shape data that the units may need.
*/
void const * TechnoTypeClass::WakeShapes = NULL;


//**********************************************************************************************
// MODULE SEPARATION -- TechnoTypeClass member functions follow.
//**********************************************************************************************


/***********************************************************************************************
 * TechnoTypeClass::TechnoTypeClass -- Constructor for techno type objects.                    *
 *                                                                                             *
 *    This is the normal constructor for techno type objects. It is called in the process of   *
 *    constructing all the object type (constant) data for the various techno type objects.    *
 *                                                                                             *
 * INPUT:   see below...                                                                       *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/19/1995 JLB : Created.                                                                 *
 *   05/11/1996 JLB : Moderated risk calc so range doesn't dominate.                           *
 *=============================================================================================*/
TechnoTypeClass::TechnoTypeClass(char const * ininame, SpeedType speed) :
	BASECLASS(ininame),
	IsDoubleOwned(false),
	IsInvisible(false),
	IsLeader(false),
	IsScanner(false),
	IsNominal(false),
	IsTurretEquipped(false),
	IsCrew(false),
	IsRepairable(true),
	IsRemappable(false),
	IsCloakable(false),
	IsSelfHealing(false),
	SelfHealingStep(-1),
	SelfHealingRate(-1),
	SelfHealingCap(-1),
	IsMechanic(false),
	IsOmniHealer(false),
	IsExploding(false),
	MZone(MZONE_NORMAL),
	ThreatRange(0),
	MaxPassengers(0),
	Size(1),
	SizeLimit(1),
	IsVehicleTransport(false),
	SightRange(0),
	Cost(0),
	Soylent(-1),
	Level(255),
	Prerequisite(),
	Risk(0),
	Reward(0),
	MaxSpeed(MPH_IMMOBILE),
	Speed(speed),
	MaxAmmo(-1),
	Ownable(0),
	CameoData(NULL),
	CameoSortOrder(0),
	Rotation(0),
	ROT(0),
	Points(0),
	CollateralDamageCoefficient(.33f),
	WalkRate(1),
	SpecialThreatValue(0),
	MyEffectivenessCoefficient(0),
	TargetEffectivenessCoefficient(0),
	TargetSpecialThreatCoefficient(0),
	TargetStrengthCoefficient(0),
	TargetDistanceCoefficient(0),
	ThreatAvoidanceCoefficient(0),
	SlowdownDistance(500),
	DeaccelerationFactor(.002),
	AccelerationFactor(.03),
	CloakingSpeed(7),
	DebrisTypes(),
	DebrisMaximums(),
	DebrisAnims(),
	DestroyAnim(),
	Locomotor(ClassID_TeleportLocomotion),
	VoxelCenterY(0),
	VoxelCenterX(0),
	Weight(1),
	PhysicalSize(2),
	InitialMission(MISSION_HUNT),
	RollAngle(DEG_TO_RAD(30)),
	PitchSpeed(.25),
	PitchAngle(DEG_TO_RAD(20)),
	BuildLimit(0x7FFFFFFF),
	Category(CATEGORY_NONE),
	Unused2(0),
	DeployTime(0),
	FireAngle(8),
	PipScale(PIPSCALE_NONE),
	Dock(),
	DeploysInto(NULL),
	UndeploysInto(NULL),
	UnloadingClass(NULL),
	VoiceSelect(),
	VoiceMove(),
	VoiceAttack(),
	VoiceDie(),
	DieSound(),
	MoveSound(),
	VoiceFeedback(),
	AuxSound1(VOC_NONE),
	AuxSound2(VOC_NONE),
	MaxDebris(0),
	MinDebris(0),
	PixelSelectionBracketDelta(0),
	LeadershipRating(5),
	IsOrganic(false),
	IsDamageSelf(false),
	IsImmuneToPsionics(false),
	IsBerserkFriendly(false),
	IsImmuneToPsionicWeapons(false),
	IsImmuneToPoison(false),
	IsCanPassiveAquire(true),
	IsCanRetaliate(true),
	IsOpportunityFire(false),
	IsWarpable(true),
	IsImmuneToRadiation(false),
	GapRadiusInCells(0),
	BombSight(0),
	DeathWeapon(NULL),
	DeathWeaponDamageModifier(1.0),
	InitialAmmo(-1),
	IsParasiteable(true),
	SuppressionThreshold(0),
	IsReselectIfLimboed(false),
	IsCanDisguise(false),
	IsPermaDisguise(false),
	IsBalloonHover(false),
	MindControlRingOffset(140),
	MindClearedSound(VOC_NONE),
	LeptonMindControlOffset(70),
	IsTeleporter(false),
	IsChronoshiftAllowed(true),
	IsChronoshiftCrushable(true),
	IsBounty(false),
	BountyDisplay(-1),
	BountyValue{0, 0, 0},
	InsigniaShapes{NULL, NULL, NULL},
	InsigniaFrame{-1, -1, -1},
	InsigniaShowEnemy(-1),
	IsVehicleThiefAllowed(true),
	BuildTimeMultipleFactory(-1.0),
	IsCrashable(true),
	PromoteVeteranSound(VOC_NONE),
	PromoteEliteSound(VOC_NONE),
	IsHealthBarHidden(false),
	EMPModifier(1.0),
	EMPThreshold(0),
	IsCanBeReversed(true),
	KeepAlive(-1),
	IsAttackFriendlies(false),
	IsAttackCursorOnFriendlies(false),
	IsDefaultToGuardArea(false),
	IsSelectableCombatant(false),
	BuildTimeMultiplier(1.0),
	ChronoInSound(VOC_NONE),
	ChronoOutSound(VOC_NONE),
	CreateSound(VOC_NONE),
	EnterTransportSound(VOC_NONE),
	LeaveTransportSound(VOC_NONE),
	VoiceEnter(VOC_NONE),
	VoiceDeploy(VOC_NONE),
	VoiceUndeploy(VOC_NONE),
	TurretRotateSound(VOC_NONE),
	IsResourceGatherer(false),
	IsResourceDestination(false),
	VoiceCapture(VOC_NONE),
	VoiceHarvest(VOC_NONE),
	ImpactLandSound(VOC_NONE),
	ImpactWaterSound(VOC_NONE),
	DamageSound(VOC_NONE),
	VoicePrimaryWeaponAttack(VOC_NONE),
	VoicePrimaryEliteWeaponAttack(VOC_NONE),
	VoiceSecondaryWeaponAttack(VOC_NONE),
	VoiceSecondaryEliteWeaponAttack(VOC_NONE),
	IsNatural(false),
	IsUnnatural(false),
	IsUnderwater(false),
	NavalTargeting(0),
	LandTargeting(0),
	Spawns(NULL),
	SpawnsNumber(0),
	SpawnRegenRate(0),
	SpawnReloadRate(0),
	SecondSpawnOffset(0, 0, 0),
	IsSpawned(false),
	IsMissileSpawn(false),
	IsDrainable(false),
	Enslaves(NULL),
	SlavesNumber(0),
	SlaveRegenRate(0),
	SlaveReloadRate(0),
	IsBunkerable(false),
	IsDistributedFire(false),
	IsCanApproachTarget(true),
	SensorsSight(0),
	IsPoweredUnit(false),
	ActivateSound(VOC_NONE),
	DeactivateSound(VOC_NONE),
	PowersUnit(NULL),
	CrashingSound(VOC_NONE),
	VoiceCrashing(VOC_NONE),
	SinkingSound(VOC_NONE),
	VoiceSinking(VOC_NONE),
	IsJumpjetDataSet(false),
	JumpjetTurnRate(4),
	JumpjetSpeed(14),
	JumpjetClimb(5),
	JumpjetHeight(500),
	JumpjetAccel(2),
	JumpjetWobbles(0.15),
	JumpjetDeviation(40),
	IsJumpjetNoWobbles(false),
	IsRevealToAll(false),
	AirstrikeTeam(0),
	EliteAirstrikeTeam(0),
	AirstrikeTeamType(NULL),
	EliteAirstrikeTeamType(NULL),
	AirstrikeRechargeTime(0),
	EliteAirstrikeRechargeTime(0),
	TurretCount(0),
	WeaponCount(0),
	IsGattling(false),
	WeaponStages(0),
	WeaponStage{},
	EliteStage{},
	RateUp(0),
	RateDown(0),
	IsGunner(false),
	IFVMode(0),
	AirRangeBonus(0),
	IsOpenTopped(false),
	OpenTransportWeapon(-1),
	FlightLevel(-1),
	IsAllowedToStartInMultiplayer(true),
	CameoFilename(""),
	TurretOffset(0),
	Explosion(),
	ScrapExplosion(),
	NaturalParticleSystem(NULL),
	NaturalParticleLocation(0,0,0),
	DamageParticleSystems(),
	DamageSmokeOffset(0,0,0),
	ShadowIndex(0),
	Capacity(0),
	TurretNotExportedOnGround(false),
	IsTypeImmune(false),
	IsCanBeHidden(true),
	IsDetectDisguise(false),
	DetectDisguiseRange(0),
	IsMoveToShroud(true),
	IsTrainable(true),
	IsNaval(false),
	RequiredHouses(-1),
	ForbiddenHouses(-1),
	AIBasePlanningSide(-1),
	IsRequiresStolenAlliedTech(false),
	IsRequiresStolenSovietTech(false),
	IsRequiresStolenThirdTech(false),
	IsDamageSparks(true),
	IsTargetLaser(false),
	IsImmuneToVeins(false),
	IsTiberiumHeal(false),
	IsCloakStop(false),
	IsTrain(false),
	IsDropship(false),
	IsToProtect(false),
	IsDisableable(true),
	IsUnbuildable(false),
	IsRadarVisible(false),
	IsNoAutoFire(false),
	IsRadarEquipped(false),
	IsRegulated(false),
	IsManualReload(false),
	IsVisibleLoad(false),
	IsLightningRod(false),
	IsHunterSeeker(false),
	IsCrusher(false),
	IsTiltsWhenCrushes(true),
	IsSubterranean(false),
	IsAutoCrush(false),
	IsAccelerates(true),
	ZFudgeCliff(10),
	ZFudgeColumn(5),
	ZFudgeTunnel(10),
	ZFudgeBridge(0)
{
	IsSentient = true;

	DebrisTypes.Clear();
	DebrisMaximums.Clear();
	DebrisAnims.Clear();
	Dock.Clear();

	for (int i = 0; i < WEAPON_SLOT_COUNT; i++) {
		Weapons[i].Weapon = NULL;
		Weapons[i].BarrelLength = 0;
		Weapons[i].BarrelThickness = 0;
		Weapons[i].FireFLH = Point3D(0,0,0);
		EliteWeapons[i] = Weapons[i];
		TurretWeapon[i] = -1;
	}

	AbstractTypePtrTracker.Add(this);
	TechnoTypes.Add(this);
}


/// <summary>
/// Destroys the object type.
/// The type removes itself from the master type lists so that nothing can look it up
/// after it is gone.
/// </summary>
TechnoTypeClass::~TechnoTypeClass(void)
{
	AbstractTypePtrTracker.Delete(this);
	TechnoTypes.Delete(this);
}


/***********************************************************************************************
 * TechnoTypeClass::Raw_Cost -- Fetches the raw (base) cost of the object.                     *
 *                                                                                             *
 *    This routine is used to find the underlying cost for this object. The underlying cost    *
 *    does not include any free items that normally come with the object when purchased        *
 *    directly. Example: The raw cost of a refinery is the normal cost minus the cost of a     *
 *    harvester.                                                                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the credit cost of the base object type.                              *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/13/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int TechnoTypeClass::Raw_Cost(void) const
{
	return(Cost);
}


/***********************************************************************************************
 * TechnoTypeClass::Get_Ownable -- Fetches the ownable bits for this object type.              *
 *                                                                                             *
 *    This routine will return the ownable bits for this object type. The ownable bits are     *
 *    a bitflag composite of the houses that can own (build) this object type.                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the ownable bits for this object type.                                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int TechnoTypeClass::Get_Ownable(void) const
{
	if (IsDoubleOwned && Session.Type != GAME_NORMAL) {
		return(0x7FFFFFFF);
	}
	return(Ownable);
}


/***********************************************************************************************
 * TechnoTypeClass::Time_To_Build -- Fetches the time to build this object.                    *
 *                                                                                             *
 *    This routine will return the time it takes to construct this object. Usually the time    *
 *    to produce is directly related to cost.                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the time to produce this object type. The time is expressed in the    *
 *          form of game ticks.                                                                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int TechnoTypeClass::Time_To_Build(void) const
{
	return(Cost * Rule->BuildSpeedBias * (TICKS_PER_MINUTE / 1000.));
}


/***********************************************************************************************
 * TechnoTypeClass::Cost_Of -- Fetches the cost of this object type.                           *
 *                                                                                             *
 *    This routine will return the cost to produce an object of this type.                     *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the cost to produce one object of this type.                          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int TechnoTypeClass::Cost_Of(HouseClass * house) const
{
	if (house != NULL) {
		return((int)(Raw_Cost() * house->CostBias * house->Cost_Multiplier(this)));
	}
	return(Raw_Cost());
}


/***********************************************************************************************
 * TechnoTypeClass::Get_Cameo_Data -- Fetches the cameo image for this object type.            *
 *                                                                                             *
 *    This routine will fetch the cameo (sidebar small image) shape of this object type.       *
 *    If there is no cameo data available (typical for non-produceable units), then NULL will  *
 *    be returned.                                                                             *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the cameo data for this object type if present.          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void const * TechnoTypeClass::Get_Cameo_Data(void) const
{
	return(CameoData);
}


/***********************************************************************************************
 * TechnoTypeClass::Repair_Cost -- Fetches the cost to repair one step.                        *
 *                                                                                             *
 *    This routine will return the cost to repair one step. At the TechnoTypeClass level,      *
 *    this merely serves as a placeholder function. The derived classes will provide a         *
 *    functional version of this routine.                                                      *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the cost to repair one step.                                          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int TechnoTypeClass::Repair_Cost(void) const
{
	int cost = (Raw_Cost()/(MaxStrength/Rule->RepairStep)) * Rule->RepairPercent;
	return(std::max(cost, 1));
}


/***********************************************************************************************
 * TechnoTypeClass::Repair_Step -- Fetches the health to repair one step.                      *
 *                                                                                             *
 *    This routine merely serves as placeholder virtual function. The various type classes     *
 *    will override this routine to return the number of health points to repair in one        *
 *    "step".                                                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of health points to repair in one step.                    *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int TechnoTypeClass::Repair_Step(void) const
{
	return(Rule->RepairStep);
}


/// <summary>
/// Returns the animations a destroyed object of this type leaves behind: its ScrapExplosion
/// list where scrap wreckage is on and the type names one, its Explosion list otherwise.
/// The result is a reference, so a caller that picks one entry and then reaches for another
/// indexes the same list both times.
/// </summary>
TypeList<AnimTypeClass const *> const & TechnoTypeClass::Explosion_Set(void) const
{
	if (Scen->Special.IsScrapMetal && ScrapExplosion.Count() > 0) {
		return(ScrapExplosion);
	}

	return(Explosion);
}


/// <summary>
/// Fetches the strength one self-healing tick restores to an object of this type.
/// </summary>
/// <returns>Returns with the type's step, else the game-wide one, never below one.</returns>
int TechnoTypeClass::Self_Heal_Step(void) const
{
	return(std::max(SelfHealingStep >= 0 ? SelfHealingStep : Rule->SelfHealStep, 1));
}


/// <summary>
/// Fetches the interval between this type's self-healing ticks, in minutes.
/// </summary>
/// <returns>Returns with the type's interval, else the game-wide one, else RepairRate.</returns>
double TechnoTypeClass::Self_Heal_Rate(void) const
{
	if (SelfHealingRate >= 0) {
		return(SelfHealingRate);
	}
	return(Rule->SelfHealRate >= 0 ? Rule->SelfHealRate : Rule->RepairRate);
}


/// <summary>
/// Fetches the health ratio at which an object of this type stops healing itself.
/// </summary>
/// <returns>Returns with the type's ceiling, else the game-wide one, else ConditionYellow.</returns>
double TechnoTypeClass::Self_Heal_Cap(void) const
{
	if (SelfHealingCap >= 0) {
		return(SelfHealingCap);
	}
	return(Rule->SelfHealCap >= 0 ? Rule->SelfHealCap : Rule->ConditionYellow);
}


/***********************************************************************************************
 * TechnoTypeClass::Is_Two_Shooter -- Determines if this object is a double shooter.           *
 *                                                                                             *
 *    Some objects fire two shots in quick succession. If this is true for this object, then   *
 *    a 'true' value will be returned from this routine.                                       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; Is this object a two shooter?                                                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/29/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool TechnoTypeClass::Is_Two_Shooter(void) const
{
	WeaponTypeClass *pri = Get_Weapon(0)->Weapon;

	if (pri != NULL) {
		WeaponTypeClass *sec = Get_Weapon(1)->Weapon;

		if (pri == sec || pri->Burst > 1) {
				return(true);
		}

		if (sec != NULL && sec->Burst > 1) {
			return(true);
		}
	}
	return(false);
}


/// <summary>
/// Does an EM pulse leave this type alone? An immune object still springs its paralyzed
/// trigger event.
/// </summary>
bool TechnoTypeClass::Is_Immune_To_EMP(void) const
{
	return(IsImmuneToEMP.value_or(false));
}


/***********************************************************************************************
 * _Scale_To_256 -- Scales a 1..100 number into a 1..255 number.                               *
 *                                                                                             *
 *    This is a helper routine that will take a decimal percentage number and convert it       *
 *    into a game based fixed point number.                                                    *
 *                                                                                             *
 * INPUT:   val   -- Decimal percent number to convert.                                        *
 *                                                                                             *
 * OUTPUT:  Returns with the decimal percent number converted to a game fixed point number.    *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/17/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
static inline int _Scale_To_256(int val)
{
	val = std::min(val, 100);
	val = std::max(val, 0);
	val = ((val * (MPH_LIGHT_SPEED + 1)) / 100);
	val = std::min<int>(val, MPH_LIGHT_SPEED);
	return(val);
}


/***********************************************************************************************
 * TechnoTypeClass::Read_INI -- Reads the techno type data from the INI database.              *
 *                                                                                             *
 *    Use this routine to fill in the data for this techno type class object from the          *
 *    database specified. Typical use of this is for the rules parsing.                        *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the INI database that the information will be lifted from.   *
 *                                                                                             *
 * OUTPUT:  bool; Was the database used to extract information? A failure (false) response     *
 *                would mean that the database didn't contain a section that applies to this   *
 *                techno class object.                                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/19/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool TechnoTypeClass::Read_INI(CCINIClass const & ini)
{
	if (BASECLASS::Read_INI(ini)) {

		// CanBeHidden comes from the art file, under the type's own name (TechnoTypeClass::LoadFromINI).
		IsCanBeHidden = ArtINI.Get_Bool(Name(), "CanBeHidden", IsCanBeHidden);
		IsTypeImmune = ini.Get_Bool(Name(), "TypeImmune", IsTypeImmune);
		IsDetectDisguise = ini.Get_Bool(Name(), "DetectDisguise", IsDetectDisguise);
		DetectDisguiseRange = ini.Get_Int(Name(), "DetectDisguiseRange", DetectDisguiseRange);
		WalkRate = ini.Get_Int(Name(), "WalkRate", WalkRate);
		IsMoveToShroud = ini.Get_Bool(Name(), "MoveToShroud", IsMoveToShroud);
		IsTrain = ini.Get_Bool(Name(), "IsTrain", IsTrain);
		IsDoubleOwned = ini.Get_Bool(Name(), "DoubleOwned", IsDoubleOwned);
		ThreatRange = ini.Get_Lepton(Name(), "GuardRange", ThreatRange);

		if (RTTI == RTTI_INFANTRYTYPE) {
			/// This routine runs before InfantryTypeClass reads its own flags, so IsCyborg is
			/// still false on the first pass through the rules.
			if (((InfantryTypeClass *)this)->IsCyborg) {
				CollateralDamageCoefficient = 0.33f;
			} else {
				CollateralDamageCoefficient = 0.66f;
			}
		} else {
			CollateralDamageCoefficient = 1.0;
		}
		CollateralDamageCoefficient = ini.Get_Float(Name(), "CollateralDamageCoefficient", CollateralDamageCoefficient);

		IsExploding = ini.Get_Bool(Name(), "Explodes", IsExploding);

		FlightLevel = ini.Get_Int(Name(), "FlightLevel", FlightLevel);
		IsDropship = ini.Get_Bool(Name(), "IsDropship", IsDropship);

		double pitch_angle = ini.Get_Float(Name(), "PitchAngle", -1);
		if (pitch_angle != -1) {
			PitchAngle = DEG_TO_RAD(pitch_angle);
		}
		double roll_angle = ini.Get_Float(Name(), "RollAngle", -1);
		if (roll_angle != -1) {
			RollAngle = DEG_TO_RAD(roll_angle);
		}

		PitchSpeed = ini.Get_Float(Name(), "PitchSpeed", PitchSpeed);
		Locomotor = ini.Get_ClassID(IniName, "Locomotor", Locomotor);
		CloakingSpeed = ini.Get_Int(Name(), "CloakingSpeed", CloakingSpeed);
		ThreatAvoidanceCoefficient = ini.Get_Float(Name(), "ThreatAvoidanceCoefficient", ThreatAvoidanceCoefficient);
		SlowdownDistance = ini.Get_Int(Name(), "SlowdownDistance", SlowdownDistance);
		DeaccelerationFactor = ini.Get_Float(Name(), "DeaccelerationFactor", DeaccelerationFactor);
		AccelerationFactor = ini.Get_Float(Name(), "AccelerationFactor", AccelerationFactor);
		Weight = ini.Get_Float(Name(), "Weight", Weight);
		PhysicalSize = ini.Get_Float(Name(), "PhysicalSize", PhysicalSize);
		MaxDebris = ini.Get_Int(Name(), "MaxDebris", MaxDebris);
		MinDebris = ini.Get_Int(Name(), "MinDebris", MinDebris);
		PixelSelectionBracketDelta = ini.Get_Int(Name(), "PixelSelectionBracketDelta", PixelSelectionBracketDelta);
		LeadershipRating = ini.Get_Int(Name(), "LeadershipRating", LeadershipRating);
		IsOrganic = ini.Get_Bool(Name(), "Organic", IsOrganic);
		IsDamageSelf = ini.Get_Bool(Name(), "DamageSelf", IsDamageSelf);
		IsImmuneToPsionics = ini.Get_Bool(Name(), "ImmuneToPsionics", IsImmuneToPsionics);
		IsBerserkFriendly = ini.Get_Bool(Name(), "BerserkFriendly", IsBerserkFriendly);
		IsImmuneToPsionicWeapons = ini.Get_Bool(Name(), "ImmuneToPsionicWeapons", IsImmuneToPsionicWeapons);
		IsImmuneToPoison = ini.Get_Bool(Name(), "ImmuneToPoison", IsImmuneToPoison);
		IsCanPassiveAquire = ini.Get_Bool(Name(), "CanPassiveAquire", IsCanPassiveAquire);
		IsCanRetaliate = ini.Get_Bool(Name(), "CanRetaliate", IsCanRetaliate);
		IsOpportunityFire = ini.Get_Bool(Name(), "OpportunityFire", IsOpportunityFire);
		IsWarpable = ini.Get_Bool(Name(), "Warpable", IsWarpable);
		IsImmuneToRadiation = ini.Get_Bool(Name(), "ImmuneToRadiation", IsImmuneToRadiation);
		GapRadiusInCells = ini.Get_Int(Name(), "GapRadiusInCells", GapRadiusInCells);
		BombSight = ini.Get_Int(Name(), "BombSight", BombSight);
		DeathWeapon = TGet_Class(ini, Name(), "DeathWeapon", DeathWeapon);
		DeathWeaponDamageModifier = ini.Get_Float(Name(), "DeathWeaponDamageModifier", DeathWeaponDamageModifier);
		InitialAmmo = ini.Get_Int(Name(), "InitialAmmo", InitialAmmo);
		IsParasiteable = ini.Get_Bool(Name(), "Parasiteable", IsParasiteable);
		SuppressionThreshold = ini.Get_Int(Name(), "SuppressionThreshold", SuppressionThreshold);
		IsReselectIfLimboed = ini.Get_Bool(Name(), "ReselectIfLimboed", IsReselectIfLimboed);
		IsCanDisguise = ini.Get_Bool(Name(), "CanDisguise", IsCanDisguise);
		IsPermaDisguise = ini.Get_Bool(Name(), "PermaDisguise", IsPermaDisguise);
		IsBalloonHover = ini.Get_Bool(Name(), "BalloonHover", IsBalloonHover);
		MindControlRingOffset = ini.Get_Int(Name(), "MindControlRingOffset", MindControlRingOffset);
		MindClearedSound = ini.Get_VocType(Name(), "MindClearedSound", MindClearedSound);
		LeptonMindControlOffset = ini.Get_Int(Name(), "LeptonMindControlOffset", LeptonMindControlOffset);
		IsTeleporter = ini.Get_Bool(Name(), "Teleporter", IsTeleporter);
		IsChronoshiftAllowed = ini.Get_Bool(Name(), "Chronoshift.Allow", IsChronoshiftAllowed);
		IsChronoshiftCrushable = ini.Get_Bool(Name(), "Chronoshift.Crushable", IsChronoshiftCrushable);
		IsBounty = ini.Get_Bool(Name(), "Bounty", IsBounty);
		if (ini.Is_Present(Name(), "Bounty.Display")) {
			BountyDisplay = ini.Get_Bool(Name(), "Bounty.Display", false) ? 1 : 0;
		}
		// Bounty.Value sets every rank's value, and a rank's own key then overrides it.
		if (ini.Is_Present(Name(), "Bounty.Value")) {
			int const value = ini.Get_Int(Name(), "Bounty.Value", 0);
			BountyValue[0] = BountyValue[1] = BountyValue[2] = value;
		}
		BountyValue[0] = ini.Get_Int(Name(), "Bounty.RookieValue", BountyValue[0]);
		BountyValue[1] = ini.Get_Int(Name(), "Bounty.VeteranValue", BountyValue[1]);
		BountyValue[2] = ini.Get_Int(Name(), "Bounty.EliteValue", BountyValue[2]);

		// The shorthand keys set every rank, and a rank's own key then overrides it.
		static char const * const RANKS[3] = {"Rookie", "Veteran", "Elite"};
		char key[48];
		char value[64];
		if (ini.Get_String(Name(), "Insignia", "", value, sizeof(value)) > 0) {
			InsigniaFile[0] = InsigniaFile[1] = InsigniaFile[2] = value;
		}
		if (ini.Is_Present(Name(), "InsigniaFrame")) {
			InsigniaFrame[0] = InsigniaFrame[1] = InsigniaFrame[2] = ini.Get_Int(Name(), "InsigniaFrame", -1);
		}
		if (ini.Get_String(Name(), "InsigniaFrames", "", value, sizeof(value)) > 0) {
			sscanf(value, "%d,%d,%d", &InsigniaFrame[0], &InsigniaFrame[1], &InsigniaFrame[2]);
		}
		for (int rank = 0; rank < 3; rank++) {
			snprintf(key, sizeof(key), "Insignia.%s", RANKS[rank]);
			if (ini.Get_String(Name(), key, "", value, sizeof(value)) > 0) {
				InsigniaFile[rank] = value;
			}
			snprintf(key, sizeof(key), "InsigniaFrame.%s", RANKS[rank]);
			InsigniaFrame[rank] = ini.Get_Int(Name(), key, InsigniaFrame[rank]);
		}
		if (ini.Is_Present(Name(), "Insignia.ShowEnemy")) {
			InsigniaShowEnemy = ini.Get_Bool(Name(), "Insignia.ShowEnemy", true) ? 1 : 0;
		}
		Load_Insignia_Shapes();

		IsVehicleThiefAllowed = ini.Get_Bool(Name(), "VehicleThief.Allowed", IsVehicleThiefAllowed);
		BuildTimeMultipleFactory = ini.Get_Float(Name(), "BuildTime.MultipleFactory", BuildTimeMultipleFactory);
		IsCrashable = ini.Get_Bool(Name(), "Crashable", IsCrashable);
		PromoteVeteranSound = ini.Get_VocType(Name(), "Promote.VeteranSound", PromoteVeteranSound);
		PromoteEliteSound = ini.Get_VocType(Name(), "Promote.EliteSound", PromoteEliteSound);
		IsHealthBarHidden = ini.Get_Bool(Name(), "HealthBar.Hide", IsHealthBarHidden);
		EMPModifier = ini.Get_Float(Name(), "EMP.Modifier", EMPModifier);
		AttachEffect.Animation = TGet_Class(ini, Name(), "AttachEffect.Animation", AttachEffect.Animation);
		AttachEffect.Duration = ini.Get_Int(Name(), "AttachEffect.Duration", AttachEffect.Duration);
		AttachEffect.IsTemporalHidesAnim = ini.Get_Bool(Name(), "AttachEffect.TemporalHidesAnim", AttachEffect.IsTemporalHidesAnim);
		AttachEffect.SpeedMultiplier = ini.Get_Float(Name(), "AttachEffect.SpeedMultiplier", AttachEffect.SpeedMultiplier);
		AttachEffect.ArmorMultiplier = ini.Get_Float(Name(), "AttachEffect.ArmorMultiplier", AttachEffect.ArmorMultiplier);
		AttachEffect.FirepowerMultiplier = ini.Get_Float(Name(), "AttachEffect.FirepowerMultiplier", AttachEffect.FirepowerMultiplier);
		AttachEffect.ROFMultiplier = ini.Get_Float(Name(), "AttachEffect.ROFMultiplier", AttachEffect.ROFMultiplier);
		AttachEffect.IsCloakable = ini.Get_Bool(Name(), "AttachEffect.Cloakable", AttachEffect.IsCloakable);
		AttachEffect.IsForceDecloak = ini.Get_Bool(Name(), "AttachEffect.ForceDecloak", AttachEffect.IsForceDecloak);
		AttachEffect.IsDiscardOnEntry = ini.Get_Bool(Name(), "AttachEffect.DiscardOnEntry", AttachEffect.IsDiscardOnEntry);
		AttachEffect.IsPenetratesIronCurtain = ini.Get_Bool(Name(), "AttachEffect.PenetratesIronCurtain", AttachEffect.IsPenetratesIronCurtain);
		AttachEffect.Delay = ini.Get_Int(Name(), "AttachEffect.Delay", AttachEffect.Delay);
		AttachEffect.InitialDelay = ini.Get_Int(Name(), "AttachEffect.InitialDelay", AttachEffect.InitialDelay);
		IsCanBeReversed = ini.Get_Bool(Name(), "CanBeReversed", IsCanBeReversed);
		IsAttackFriendlies = ini.Get_Bool(Name(), "AttackFriendlies", IsAttackFriendlies);
		IsAttackCursorOnFriendlies = ini.Get_Bool(Name(), "AttackCursorOnFriendlies", IsAttackCursorOnFriendlies);
		IsDefaultToGuardArea = ini.Get_Bool(Name(), "DefaultToGuardArea", IsDefaultToGuardArea);
		IsSelectableCombatant = ini.Get_Bool(Name(), "IsSelectableCombatant", IsSelectableCombatant);
		BuildTimeMultiplier = ini.Get_Float(Name(), "BuildTimeMultiplier", BuildTimeMultiplier);
		if (ini.Get_String(Name(), "GroupAs", "", value, sizeof(value)) > 0) {
			GroupAs = value;
		}
		if (ini.Is_Present(Name(), "KeepAlive")) {
			KeepAlive = ini.Get_Bool(Name(), "KeepAlive", false) ? 1 : 0;
			Rule->IsKeepAliveSet = true;
		}
		if (ini.Get_String(Name(), "ReversedAs", "", value, sizeof(value)) > 0) {
			ReversedAs = value;
		}
		if (ini.Get_String(Name(), "EMP.Threshold", "", value, sizeof(value)) > 0) {
			if (stricmp(value, "inair") == 0) {
				EMPThreshold = -1;
			} else if (stricmp(value, "yes") == 0 || stricmp(value, "true") == 0) {
				EMPThreshold = 1;
			} else if (stricmp(value, "no") == 0 || stricmp(value, "false") == 0) {
				EMPThreshold = 0;
			} else {
				EMPThreshold = atoi(value);
			}
		}
		ChronoInSound = ini.Get_VocType(Name(), "ChronoInSound", ChronoInSound);
		ChronoOutSound = ini.Get_VocType(Name(), "ChronoOutSound", ChronoOutSound);
		CreateSound = ini.Get_VocType(Name(), "CreateSound", CreateSound);
		EnterTransportSound = ini.Get_VocType(Name(), "EnterTransportSound", EnterTransportSound);
		LeaveTransportSound = ini.Get_VocType(Name(), "LeaveTransportSound", LeaveTransportSound);
		VoiceEnter = ini.Get_VocType(Name(), "VoiceEnter", VoiceEnter);
		VoiceDeploy = ini.Get_VocType(Name(), "VoiceDeploy", VoiceDeploy);
		VoiceUndeploy = ini.Get_VocType(Name(), "VoiceUndeploy", VoiceUndeploy);
		TurretRotateSound = ini.Get_VocType(Name(), "TurretRotateSound", TurretRotateSound);
		IsResourceGatherer = ini.Get_Bool(Name(), "ResourceGatherer", IsResourceGatherer);
		IsResourceDestination = ini.Get_Bool(Name(), "ResourceDestination", IsResourceDestination);
		VoiceCapture = ini.Get_VocType(Name(), "VoiceCapture", VoiceCapture);
		VoiceHarvest = ini.Get_VocType(Name(), "VoiceHarvest", VoiceHarvest);
		ImpactLandSound = ini.Get_VocType(Name(), "ImpactLandSound", ImpactLandSound);
		ImpactWaterSound = ini.Get_VocType(Name(), "ImpactWaterSound", ImpactWaterSound);
		DamageSound = ini.Get_VocType(Name(), "DamageSound", DamageSound);
		IsNatural = ini.Get_Bool(Name(), "Natural", IsNatural);
		IsUnnatural = ini.Get_Bool(Name(), "Unnatural", IsUnnatural);
		IsUnderwater = ini.Get_Bool(Name(), "Underwater", IsUnderwater);
		NavalTargeting = ini.Get_Int(Name(), "NavalTargeting", NavalTargeting);
		LandTargeting = ini.Get_Int(Name(), "LandTargeting", LandTargeting);
		VoicePrimaryWeaponAttack = ini.Get_VocType(Name(), "VoicePrimaryWeaponAttack", VoicePrimaryWeaponAttack);
		VoicePrimaryEliteWeaponAttack = ini.Get_VocType(Name(), "VoicePrimaryEliteWeaponAttack", VoicePrimaryEliteWeaponAttack);
		VoiceSecondaryWeaponAttack = ini.Get_VocType(Name(), "VoiceSecondaryWeaponAttack", VoiceSecondaryWeaponAttack);
		VoiceSecondaryEliteWeaponAttack = ini.Get_VocType(Name(), "VoiceSecondaryEliteWeaponAttack", VoiceSecondaryEliteWeaponAttack);
		Spawns = TGet_Class(ini, Name(), "Spawns", Spawns);
		SpawnsNumber = ini.Get_Int(Name(), "SpawnsNumber", SpawnsNumber);
		SpawnRegenRate = ini.Get_Int(Name(), "SpawnRegenRate", SpawnRegenRate);
		SpawnReloadRate = ini.Get_Int(Name(), "SpawnReloadRate", SpawnReloadRate);
		SecondSpawnOffset = ini.Get_Point(Name(), "SecondSpawnOffset", SecondSpawnOffset);
		IsSpawned = ini.Get_Bool(Name(), "Spawned", IsSpawned);
		IsMissileSpawn = ini.Get_Bool(Name(), "MissileSpawn", IsMissileSpawn);
		IsDrainable = ini.Get_Bool(Name(), "Drainable", IsDrainable);
		Enslaves = TGet_Class(ini, Name(), "Enslaves", Enslaves);
		SlavesNumber = ini.Get_Int(Name(), "SlavesNumber", SlavesNumber);
		SlaveRegenRate = ini.Get_Int(Name(), "SlaveRegenRate", SlaveRegenRate);
		SlaveReloadRate = ini.Get_Int(Name(), "SlaveReloadRate", SlaveReloadRate);
		IsBunkerable = ini.Get_Bool(Name(), "Bunkerable", IsBunkerable);
		IsDistributedFire = ini.Get_Bool(Name(), "DistributedFire", IsDistributedFire);
		IsCanApproachTarget = ini.Get_Bool(Name(), "CanApproachTarget", IsCanApproachTarget);
		SensorsSight = ini.Get_Int(Name(), "SensorsSight", SensorsSight);
		IsPoweredUnit = ini.Get_Bool(Name(), "PoweredUnit", IsPoweredUnit);
		ActivateSound = ini.Get_VocType(Name(), "ActivateSound", ActivateSound);
		DeactivateSound = ini.Get_VocType(Name(), "DeactivateSound", DeactivateSound);
		PowersUnit = TGet_Class(ini, Name(), "PowersUnit", PowersUnit);
		CrashingSound = ini.Get_VocType(Name(), "CrashingSound", CrashingSound);
		VoiceCrashing = ini.Get_VocType(Name(), "VoiceCrashing", VoiceCrashing);
		SinkingSound = ini.Get_VocType(Name(), "SinkingSound", SinkingSound);
		VoiceSinking = ini.Get_VocType(Name(), "VoiceSinking", VoiceSinking);

		// The first read takes its jumpjet defaults from [JumpjetControls], read before the types.
		if (!IsJumpjetDataSet) {
			IsJumpjetDataSet = true;
			JumpjetTurnRate = Rule->JumpjetTurnRate;
			JumpjetSpeed = Rule->JumpjetSpeed;
			JumpjetClimb = Rule->JumpjetClimb;
			JumpjetHeight = Rule->JumpjetCruiseHeight;
			JumpjetAccel = Rule->JumpjetAcceleration;
			JumpjetWobbles = Rule->JumpjetWobblesPerSecond;
			JumpjetDeviation = Rule->JumpjetWobbleDeviation;
		}
		JumpjetTurnRate = ini.Get_Int(Name(), "JumpjetTurnRate", JumpjetTurnRate);
		JumpjetSpeed = ini.Get_Int(Name(), "JumpjetSpeed", JumpjetSpeed);
		JumpjetClimb = ini.Get_Float(Name(), "JumpjetClimb", JumpjetClimb);
		JumpjetHeight = ini.Get_Int(Name(), "JumpjetHeight", JumpjetHeight);
		JumpjetAccel = ini.Get_Float(Name(), "JumpjetAccel", JumpjetAccel);
		JumpjetWobbles = ini.Get_Float(Name(), "JumpjetWobbles", JumpjetWobbles);
		JumpjetDeviation = ini.Get_Int(Name(), "JumpjetDeviation", JumpjetDeviation);
		IsJumpjetNoWobbles = ini.Get_Bool(Name(), "JumpjetNoWobbles", IsJumpjetNoWobbles);
		IsRevealToAll = ini.Get_Bool(Name(), "RevealToAll", IsRevealToAll);
		AirstrikeTeam = ini.Get_Int(Name(), "AirstrikeTeam", AirstrikeTeam);
		EliteAirstrikeTeam = ini.Get_Int(Name(), "EliteAirstrikeTeam", EliteAirstrikeTeam);
		AirstrikeTeamType = TGet_Class(ini, Name(), "AirstrikeTeamType", AirstrikeTeamType);
		EliteAirstrikeTeamType = TGet_Class(ini, Name(), "EliteAirstrikeTeamType", EliteAirstrikeTeamType);
		AirstrikeRechargeTime = ini.Get_Int(Name(), "AirstrikeRechargeTime", AirstrikeRechargeTime);
		EliteAirstrikeRechargeTime = ini.Get_Int(Name(), "EliteAirstrikeRechargeTime", EliteAirstrikeRechargeTime);
		DebrisTypes = TGet_TypeList<VoxelAnimTypeClass>(ini, IniName, "DebrisTypes", DebrisTypes);
		DebrisMaximums = ini.Get_IntList(IniName, "DebrisMaximums", DebrisMaximums);
		DebrisAnims = TGet_TypeList<AnimTypeClass>(ini, IniName, "DebrisAnims", DebrisAnims);
		DestroyAnim = TGet_TypeList<AnimTypeClass>(ini, IniName, "DestroyAnim", DestroyAnim);
		TurretCount = ini.Get_Int(Name(), "TurretCount", TurretCount);
		WeaponCount = ini.Get_Int(Name(), "WeaponCount", WeaponCount);
		if (!Has_Multiple_Turrets()) {
			Weapons[0].Weapon = TGet_Class(ini, Name(), "Primary", Weapons[0].Weapon);
			Weapons[1].Weapon = TGet_Class(ini, Name(), "Secondary", Weapons[1].Weapon);
			EliteWeapons[0].Weapon = TGet_Class(ini, Name(), "ElitePrimary", EliteWeapons[0].Weapon);
			EliteWeapons[1].Weapon = TGet_Class(ini, Name(), "EliteSecondary", EliteWeapons[1].Weapon);
		} else {
			char buf[32];
			for (int i = 0; i < std::min<int>(WeaponCount, WEAPON_SLOT_COUNT); i++) {
				sprintf(buf, "Weapon%d", i + 1);
				Weapons[i].Weapon = TGet_Class(ini, Name(), buf, Weapons[i].Weapon);
				sprintf(buf, "EliteWeapon%d", i + 1);
				EliteWeapons[i].Weapon = TGet_Class(ini, Name(), buf, EliteWeapons[i].Weapon);
			}
		}
		IsGunner = ini.Get_Bool(Name(), "Gunner", IsGunner);
		IFVMode = ini.Get_Int(Name(), "IFVMode", IFVMode);
		AirRangeBonus = ini.Get_Lepton(Name(), "AirRangeBonus", AirRangeBonus);
		IsOpenTopped = ini.Get_Bool(Name(), "OpenTopped", IsOpenTopped);
		OpenTransportWeapon = ini.Get_Int(Name(), "OpenTransportWeapon", OpenTransportWeapon);
		IsGattling = ini.Get_Bool(Name(), "IsGattling", IsGattling);
		WeaponStages = ini.Get_Int(Name(), "WeaponStages", WeaponStages);
		RateUp = ini.Get_Int(Name(), "RateUp", RateUp);
		RateDown = ini.Get_Int(Name(), "RateDown", RateDown);
		if (IsGattling && WeaponStages > 1) {
			char buf[32];
			for (int i = 0; i < std::min<int>(WeaponStages, WEAPON_STAGE_COUNT); i++) {
				sprintf(buf, "Stage%d", i + 1);
				WeaponStage[i] = ini.Get_Int(Name(), buf, WeaponStage[i]);
				sprintf(buf, "EliteStage%d", i + 1);
				EliteStage[i] = ini.Get_Int(Name(), buf, EliteStage[i]);
			}
		}
		VoiceMove = ini.Get_VocType_List(ini, IniName, "VoiceMove", VoiceMove);
		VoiceSelect = ini.Get_VocType_List(ini, IniName, "VoiceSelect", VoiceSelect);
		VoiceAttack = ini.Get_VocType_List(ini, IniName, "VoiceAttack", VoiceAttack);
		VoiceDie = ini.Get_VocType_List(ini, IniName, "VoiceDie", VoiceDie);
		DieSound = ini.Get_VocType_List(ini, IniName, "DieSound", DieSound);
		MoveSound = ini.Get_VocType_List(ini, IniName, "MoveSound", MoveSound);
		VoiceFeedback = ini.Get_VocType_List(ini, IniName, "VoiceFeedback", VoiceFeedback);
		AuxSound1 = ini.Get_VocType(Name(), "AuxSound1", AuxSound1);
		AuxSound2 = ini.Get_VocType(Name(), "AuxSound2", AuxSound2);
		IsCloakStop = ini.Get_Bool(Name(), "CloakStop", IsCloakStop);
		Capacity = ini.Get_Int(Name(), "Storage", Capacity);
		BuildLimit = ini.Get_Int(Name(), "BuildLimit", BuildLimit);
		CameoSortOrder = ini.Get_Int(Name(), "CameoSortOrder", CameoSortOrder);
		Category = ini.Get_CategoryType(Name(), "Category", Category);
		Dock = TGet_TypeList<BuildingTypeClass>(ini, Name(), "Dock", Dock);
		DeploysInto = TGet_Class(ini, Name(), "DeploysInto", DeploysInto);
		UndeploysInto = TGet_Class(ini, Name(), "UndeploysInto", UndeploysInto);
		UnloadingClass = TGet_Class(ini, Name(), "UnloadingClass", UnloadingClass);
		IsLightningRod = ini.Get_Bool(Name(), "LightningRod", IsLightningRod);
		IsManualReload = ini.Get_Bool(Name(), "ManualReload", IsManualReload);
		IsRadarEquipped = ini.Get_Bool(Name(), "TurretSpins", IsRadarEquipped);
		IsTurretEquipped = ini.Get_Bool(Name(), "Turret", IsTurretEquipped);
		Explosion = TGet_TypeList<AnimTypeClass>(ini, Name(), "Explosion", Explosion);
		ScrapExplosion = TGet_TypeList<AnimTypeClass>(ini, Name(), "ScrapExplosion", ScrapExplosion);
		NaturalParticleSystem = TGet_Class(ini, Name(), "NaturalParticleSystem", NaturalParticleSystem);
		NaturalParticleLocation = ini.Get_Offset(Name(), "NaturalParticleLocation", NaturalParticleLocation);
		DamageParticleSystems = TGet_TypeList<ParticleSystemTypeClass>(ini, Name(), "DamageParticleSystems", DamageParticleSystems);
		DamageSmokeOffset = ini.Get_Offset(Name(), "DamageSmokeOffset", DamageSmokeOffset);
		IsNominal = ini.Get_Bool(Name(), "Nominal", IsNominal);
		IsCloakable = ini.Get_Bool(Name(), "Cloakable", IsCloakable);
		IsScanner = ini.Get_Bool(Name(), "Sensors", IsScanner);
		PipScale = ini.Get_PipScaleType(Name(), "PipScale", PipScale);
		if (ini.Is_Present(Name(), "MaxPips")) {
			MaxPips = std::max(ini.Get_Int(Name(), "MaxPips", 0), 0);
		}
		Prerequisite = ini.Get_BuildingType_List(ini, IniName, "Prerequisite", Prerequisite);
		SightRange = ini.Get_Int(Name(), "Sight", SightRange);
		Level = ini.Get_Int(Name(), "TechLevel", Level);
		int maxspeed = ini.Get_Int(Name(), "Speed", -1);
		if (maxspeed != -1) {
			MaxSpeed = MPHType(_Scale_To_256(maxspeed));
		}
		Cost = ini.Get_Int(Name(), "Cost", Cost);
		Soylent = ini.Get_Int(Name(), "Soylent", Soylent);
		MaxAmmo = ini.Get_Int(Name(), "Ammo", MaxAmmo);
		Reward = Points = ini.Get_Int(Name(), "Points", Points);
		Risk = ini.Get_Int(Name(), "ThreatPosed", Risk);
		Ownable = ini.Get_Owners(Name(), "Owner", Ownable);
		IsTrainable = ini.Get_Bool(Name(), "Trainable", IsTrainable);
		IsNaval = ini.Get_Bool(Name(), "Naval", IsNaval);
		RequiredHouses = ini.Get_Owners(Name(), "RequiredHouses", RequiredHouses);
		ForbiddenHouses = ini.Get_Owners(Name(), "ForbiddenHouses", ForbiddenHouses);
		AIBasePlanningSide = ini.Get_Int(Name(), "AIBasePlanningSide", AIBasePlanningSide);
		IsRequiresStolenAlliedTech = ini.Get_Bool(Name(), "RequiresStolenAlliedTech", IsRequiresStolenAlliedTech);
		IsRequiresStolenSovietTech = ini.Get_Bool(Name(), "RequiresStolenSovietTech", IsRequiresStolenSovietTech);
		IsRequiresStolenThirdTech = ini.Get_Bool(Name(), "RequiresStolenThirdTech", IsRequiresStolenThirdTech);
		IsCrew = ini.Get_Bool(Name(), "Crewed", IsCrew);
		IsRepairable = ini.Get_Bool(Name(), "Repairable", IsRepairable);
		IsInvisible = ini.Get_Bool(Name(), "Invisible", IsInvisible);
		IsRadarVisible = ini.Get_Bool(Name(), "RadarVisible", IsRadarVisible);
		IsSelfHealing = ini.Get_Bool(Name(), "SelfHealing", IsSelfHealing);
		SelfHealingStep = ini.Get_Int(Name(), "SelfHealingStep", SelfHealingStep);
		SelfHealingRate = ini.Get_Float(Name(), "SelfHealingRate", SelfHealingRate);
		SelfHealingCap = ini.Get_Float(Name(), "SelfHealingCap", SelfHealingCap);
		IsMechanic = ini.Get_Bool(Name(), "Mechanic", IsMechanic);
		IsOmniHealer = ini.Get_Bool(Name(), "OmniHealer", IsOmniHealer);
		IsNoAutoFire = ini.Get_Bool(Name(), "NoAutoFire", IsNoAutoFire);
		ROT = ini.Get_Int(Name(), "ROT", ROT);
		MaxPassengers = ini.Get_Int(Name(), "Passengers", MaxPassengers);
		Size = ini.Get_Int(Name(), "Size", Size);
		SizeLimit = ini.Get_Int(Name(), "SizeLimit", SizeLimit);
		IsVehicleTransport = ini.Get_Bool(Name(), "IsVehicleTransport", IsVehicleTransport);
		FireAngle = ini.Get_Int(Name(), "FireAngle", FireAngle);
		DeployTime = ini.Get_Float(Name(), "DeployTime", DeployTime);
		IsDisableable = ini.Get_Bool(Name(), "Disableable", IsDisableable);
		IsToProtect = ini.Get_Bool(Name(), "ToProtect", IsToProtect);
		IsTiberiumHeal = ini.Get_Bool(Name(), "TiberiumHeal", IsTiberiumHeal);
		IsImmuneToVeins = ini.Get_Bool(Name(), "ImmuneToVeins", IsImmuneToVeins);
		if (ini.Is_Present(Name(), "ImmuneToEMP")) {
			IsImmuneToEMP = ini.Get_Bool(Name(), "ImmuneToEMP", false);
		}
		IsAllowedToStartInMultiplayer = ini.Get_Bool(Name(), "AllowedToStartInMultiplayer", IsAllowedToStartInMultiplayer);
		IsTargetLaser = ini.Get_Bool(Name(), "TargetLaser", IsTargetLaser);
		IsHunterSeeker = ini.Get_Bool(Name(), "HunterSeeker", IsHunterSeeker);
		IsCrusher = ini.Get_Bool(Name(), "Crusher", IsCrusher);
		IsAutoCrush = ini.Get_Bool(Name(), "AutoCrush", IsAutoCrush);
		IsTiltsWhenCrushes = ini.Get_Bool(Name(), "TiltsWhenCrushes", IsTiltsWhenCrushes);
		IsAccelerates = ini.Get_Bool(Name(), "Accelerates", IsAccelerates);
		ZFudgeCliff = ini.Get_Int(Name(), "ZFudgeCliff", ZFudgeCliff);
		ZFudgeColumn = ini.Get_Int(Name(), "ZFudgeColumn", ZFudgeColumn);
		ZFudgeTunnel = ini.Get_Int(Name(), "ZFudgeTunnel", ZFudgeTunnel);
		ZFudgeBridge = ini.Get_Int(Name(), "ZFudgeBridge", ZFudgeBridge);
		VeteranAbilities = ini.Get_Abilities(Name(), "VeteranAbilities", VeteranAbilities);
		EliteAbilities = ini.Get_Abilities(Name(), "EliteAbilities", EliteAbilities);
		MyEffectivenessCoefficient = ini.Get_Float(Name(), "MyEffectivenessCoefficient", MyEffectivenessCoefficient == 0 ? Rule->MyEffectivenessCoefficientDefault : MyEffectivenessCoefficient);
		TargetEffectivenessCoefficient = ini.Get_Float(Name(), "TargetEffectivenessCoefficient", TargetEffectivenessCoefficient == 0 ? Rule->TargetEffectivenessCoefficientDefault : TargetEffectivenessCoefficient);
		TargetSpecialThreatCoefficient = ini.Get_Float(Name(), "TargetSpecialThreatCoefficient", TargetSpecialThreatCoefficient == 0 ? Rule->TargetSpecialThreatCoefficientDefault : TargetSpecialThreatCoefficient);
		TargetStrengthCoefficient = ini.Get_Float(Name(), "TargetStrengthCoefficient", TargetStrengthCoefficient == 0 ? Rule->TargetStrengthCoefficientDefault : TargetStrengthCoefficient);
		TargetDistanceCoefficient = ini.Get_Float(Name(), "TargetDistanceCoefficient", TargetDistanceCoefficient == 0 ? Rule->TargetDistanceCoefficientDefault : TargetDistanceCoefficient);
		SpecialThreatValue = ini.Get_Float(Name(), "SpecialThreatValue", SpecialThreatValue);

		IsLeader = false;
		if (Weapons[0].Weapon != NULL && Weapons[0].Weapon->Attack > 0) {
			IsLeader = true;
		}

		char filename[512];
		_makepath(filename, NULL, NULL, Graphic_Name(), ".SHP");
		ImageData = MFCD::Retrieve(filename);

		FireAngle = ArtINI.Get_Angle(Graphic_Name(), "FireAngle", FireAngle);
		TurretOffset = ArtINI.Get_Int(Graphic_Name(), "TurretOffset", TurretOffset);
		Rotation = ArtINI.Get_Int(Graphic_Name(), "RotCount", Rotation);
		IsRemappable = ArtINI.Get_Bool(Graphic_Name(), "Remapable", IsRemappable);
		IsRegulated = ArtINI.Get_Bool(Graphic_Name(), "Normalized", IsRegulated);
		IsVisibleLoad = ArtINI.Get_Bool(Graphic_Name(), "VisibleLoad", IsVisibleLoad);
		ShadowIndex = ArtINI.Get_Int(Graphic_Name(), "ShadowIndex", ShadowIndex);

		char pcx[64];
		ArtINI.Get_String(Graphic_Name(), "CameoPCX", "", pcx, sizeof(pcx));
		CameoPCX = pcx;

		TStringID<24> cameo;
		if (ArtINI.Get_String(Graphic_Name(), "Cameo", "", cameo) > 0) {
			CameoFilename = cameo;
			_makepath(filename, NULL, NULL, CameoFilename, ".SHP");
			CameoData = (ShapeSet const *)MFCD::Retrieve(filename);
		}
		if (CameoData == NULL) {
			CameoData = (ShapeSet const *)MFCD::Retrieve("XXICON.SHP");
		}

		// An elite weapon fires from where its normal counterpart does unless the art says otherwise.
		if (!Has_Multiple_Turrets()) {
			Weapons[0].FireFLH = ArtINI.Get_Point(Graphic_Name(), "PrimaryFireFLH", Weapons[0].FireFLH);
			Weapons[0].BarrelLength = ArtINI.Get_Int(Graphic_Name(), "PBarrelLength", Weapons[0].BarrelLength);
			Weapons[0].BarrelThickness = ArtINI.Get_Int(Graphic_Name(), "PBarrelThickness", Weapons[0].BarrelThickness);
			Weapons[1].FireFLH = ArtINI.Get_Point(Graphic_Name(), "SecondaryFireFLH", Weapons[1].FireFLH);
			Weapons[1].BarrelLength = ArtINI.Get_Int(Graphic_Name(), "SBarrelLength", Weapons[1].BarrelLength);
			Weapons[1].BarrelThickness = ArtINI.Get_Int(Graphic_Name(), "SBarrelThickness", Weapons[1].BarrelThickness);
			EliteWeapons[0].FireFLH = ArtINI.Get_Point(Graphic_Name(), "ElitePrimaryFireFLH", Weapons[0].FireFLH);
			EliteWeapons[0].BarrelLength = ArtINI.Get_Int(Graphic_Name(), "ElitePBarrelLength", Weapons[0].BarrelLength);
			EliteWeapons[0].BarrelThickness = ArtINI.Get_Int(Graphic_Name(), "ElitePBarrelThickness", Weapons[0].BarrelThickness);
			EliteWeapons[1].FireFLH = ArtINI.Get_Point(Graphic_Name(), "EliteSecondaryFireFLH", Weapons[1].FireFLH);
			EliteWeapons[1].BarrelLength = ArtINI.Get_Int(Graphic_Name(), "EliteSBarrelLength", Weapons[1].BarrelLength);
			EliteWeapons[1].BarrelThickness = ArtINI.Get_Int(Graphic_Name(), "EliteSBarrelThickness", Weapons[1].BarrelThickness);
		} else {
			char buf[32];
			for (int i = 0; i < std::min<int>(WeaponCount, WEAPON_SLOT_COUNT); i++) {
				sprintf(buf, "Weapon%dFLH", i + 1);
				Weapons[i].FireFLH = ArtINI.Get_Point(Graphic_Name(), buf, Weapons[i].FireFLH);
				sprintf(buf, "EliteWeapon%dFLH", i + 1);
				EliteWeapons[i].FireFLH = ArtINI.Get_Point(Graphic_Name(), buf, Weapons[i].FireFLH);
			}
		}

		TurretNotExportedOnGround = ArtINI.Get_Bool(Graphic_Name(), "TurretNotExportedOnGround", TurretNotExportedOnGround);

		/*
		**	Check to see what zone this object should recognize.
		*/
		MZone = ini.Get_MZoneType(Name(), "MovementZone", MZone);
		Speed = ini.Get_SpeedType(Name(), "SpeedType", Speed);
		IsSubterranean = MZone == MZONE_SUBTERANNEAN;

		if (IsVoxel) {
			Fetch_Voxel_Image();
		} else if (IsTurretEquipped) {
			Fetch_Aux_Voxel_Image();
		}

		if (Voxel.VoxLib != NULL && !Voxel.VoxLib->Load_Failed()) {
			VoxelCenterY = Voxel.VoxLib->Get_Layer_Info(0, 0).YSize * 0.5f + 0.5;
			VoxelCenterX = Voxel.VoxLib->Get_Layer_Info(0, 0).XSize * 0.5f;
		}

		return(true);
	}
	return(false);
}


/// <summary>
/// Determines if this object type may legally be placed at the specified location.
/// Every square of the object's foundation is examined for obstacles -- buildings are
/// weighed against the house's own build restrictions, everything else merely has to fit.
/// A building that paves what it covers is more forgiving, since it only needs one square
/// of the foundation to be free.
/// </summary>
/// <param name="pos">The cell to anchor the object's foundation at.</param>
/// <param name="house">The house that wishes to do the placing.</param>
/// <returns>bool; May the object be placed at this location?</returns>
bool TechnoTypeClass::Legal_Placement(Cell const & pos, HouseClass * house) const
{
	if (pos == CELL_NONE) return(0);

	/*
	**	Normal buildings must check to see that every foundation square is free of
	**	obstacles. If this check passes for all foundation squares, only then does the
	**	routine return that it is legal to place.
	*/
	Cell const * offset = Occupy_List(true);
	bool build = (RTTI == RTTI_BUILDINGTYPE);
	bool any_blocked = false;
	bool any_clear = false;

	BuildingTypeClass * bptr = (BuildingTypeClass *)this;

	while (offset != NULL && *offset != REFRESH_EOL) {
		Cell cell = pos + *offset++;

		if (build) {
			if (!Map[cell].Is_Clear_To_Build(bptr->Speed, bptr, house)) {
				any_blocked = true;
			} else {
				any_clear = true;
			}
		} else {
			if (!Map[cell].Is_Clear_To_Move(Speed, false, false)) {
				any_blocked = true;
			} else {
				any_clear = true;
			}
		}
	}

	if (build && bptr->ToTile != NULL) {
		return(any_clear);
	}
	return(any_blocked == 0);
}


/// <summary>
/// Fetches the number of pips this object type displays.
/// The pip scale assigned in the rules decides both what the pips stand for -- ammo,
/// passengers, tiberium, power or charge -- and how many of them there can be.
/// </summary>
/// <returns>Returns with the maximum pip count, or zero if this object type shows none.</returns>
int TechnoTypeClass::Max_Pips(void) const
{
	switch (PipScale) {
		case PIPSCALE_POWER:
			return(MaxPips.value_or(10));

		case PIPSCALE_AMMO:
			return(std::min(MaxAmmo, MaxPips.value_or(5)));

		case PIPSCALE_TIBERIUM:
			return(MaxPips.value_or(5));

		case PIPSCALE_PASSENGERS:
			return(std::min(MaxPassengers, MaxPips.value_or(5)));

		case PIPSCALE_CHARGE:
			return(MaxPips.value_or(8));
	}
	return(0);
}


/***********************************************************************************************
 * TechnoClass::In_Range -- Determines if specified target is within weapon range.             *
 *                                                                                             *
 *    This routine is used to compare the distance to the specified target with the range      *
 *    of the weapon. If the target is outside of weapon range, then false is returned.         *
 *                                                                                             *
 * INPUT:   target   -- The target to check if it is within weapon range.                      *
 *                                                                                             *
 *          which    -- Which weapon to use in determining range. 0=primary, 1=secondary.      *
 *                                                                                             *
 * OUTPUT:  bool; Is the specified target within weapon range?                                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/14/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool TechnoTypeClass::In_Range(Coord const & coord, AbstractClass * target, WeaponTypeClass * weapon, int bonus) const
{
//	//assert(IsActive);

	bool is_within_ground_firing_angle;

	// A weapon with Range=-2 reaches anything (TechnoClass::InRange, 0x6F7220).
	if (target != NULL && weapon != NULL && weapon->Range == -2 * CELL_LEPTON_W) {
		return(true);
	}

	if (target != NULL && weapon != NULL) {
		// A target in the air is reached AirRangeBonus farther (TechnoClass::InRange, 0x6F7274).
		int range = weapon->Range + bonus;
		if (target->In_Air()) {
			range += AirRangeBonus;
		}
		Coord tcoord = target->Center_Coord();
		int minrange = weapon->MinimumRange;

		if (minrange && (coord - tcoord).Length() < minrange) {
			return(false);
		}

		if (weapon->Bullet->IsArcing) {
			int dist = (Point2D(coord) - Point2D(tcoord)).Length();
			if (dist > range) {
				return(false);
			}
			int height = tcoord.Z - coord.Z;
			double gravity = weapon->Bullet->IsFloater ? Get_Floater_Gravity() : Rule->Gravity;
			if (!Is_Projectile_Trajectory_Valid(weapon->MaxSpeed, dist, height, gravity) || (Map[tcoord].IsUnderBridge && tcoord.Z - coord.Z >= 3 * LEVEL_LEPTON_H)) {
				return(false);
			}
			Coord dist2d = Coord(coord.X - tcoord.X, coord.Y - tcoord.Y, 0);
			is_within_ground_firing_angle = coord.Z - tcoord.Z < dist2d.Length();
		} else {

			BuildingClass const * building = target->As_BuildingClass();
			if (building != NULL) {
				range += ((building->Class->Width() + building->Class->Height()) * (CELL_LEPTON_W / 4));
			}

			int cdist;
			if (RTTI != RTTI_AIRCRAFTTYPE) {
				cdist = (coord - tcoord).Length();
			} else {
				cdist = (Coord(Point2D(coord) - Point2D(tcoord), 0)).Length();
			}

			if (cdist > range) {
				return(false);
			}

			if (Map[coord].IsUnderBridge) {
				int height = BRIDGE_LEPTON_HEIGHT + Map.Get_Height_GL(coord);
				if (coord.Z < height && tcoord.Z >= height) {
					return(false);
				}
			}

			/*
			 * A ground weapon cannot hit a target that is too high above it -- the
			 * target must be no higher than the horizontal distance to it.
			 */
			Coord coord1 = coord;
			Coord coord2 = tcoord;
			if (!weapon->Bullet->IsAntiAircraft) {
				Point2D dist2d = Point2D(coord1 - coord2);
				if (tcoord.Z - coord.Z >= dist2d.Length()) {
					return(false);
				}
			}

			Point2D dist2d = Point2D(coord1 - coord2);
			is_within_ground_firing_angle = coord.Z - tcoord.Z < dist2d.Length();
		}

		if (!is_within_ground_firing_angle) {
			int hoffset = Map[coord].IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0;
			if (coord.Z <= hoffset + Map.Get_Height_GL(coord)) {
				return(false);
			}
		}
		return(true);
	}

	return(false);
}


/// <summary>
/// Re-attaches the artwork this techno type names.
/// Artwork is never written to a save game, so the shape and the sidebar cameo are
/// re-fetched from the mix files once the members have been read.
/// </summary>
void TechnoTypeClass::Post_Load(void)
{
	BASECLASS::Post_Load();

		char buffer[256];
		char fname[_MAX_PATH];

		_makepath(fname, NULL, NULL, (const char *)GraphicName, ".SHP");
		ImageData = MFCD::Retrieve(fname);

		ArtINI.Get_String((const char *)GraphicName, "Cameo", "XXICON", buffer, sizeof(buffer));
		if (stricmp(buffer, "XXICON") == 0) {
			ArtINI.Get_String((const char *)IniName, "Cameo", "XXICON", buffer, sizeof(buffer));
		}
		_makepath(fname, NULL, NULL, buffer, ".SHP");
		CameoData = (const ShapeSet *)MFCD::Retrieve(fname);

		ArtINI.Get_String((const char *)GraphicName, "CameoPCX", "", buffer, sizeof(buffer));
		CameoPCX = buffer;

		Load_Insignia_Shapes();
}


std::string TechnoTypeClass::Select_Group(void) const
{
	std::string group = GroupAs.empty() ? std::string(Name()) : GroupAs;
	for (char & letter : group) {
		letter = (char)toupper((unsigned char)letter);
	}
	return(group);
}


void TechnoTypeClass::Load_Insignia_Shapes(void)
{
	for (int rank = 0; rank < 3; rank++) {
		InsigniaShapes[rank] = NULL;
		if (!InsigniaFile[rank].empty()) {
			std::string const filename = InsigniaFile[rank] + ".SHP";
			InsigniaShapes[rank] = (ShapeSet const *)MFCD::Retrieve(filename.c_str());
		}
	}
}


/// <summary>
/// Lists the members every techno type carries.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void TechnoTypeClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	stream.Serialize(CollateralDamageCoefficient);
	stream.Serialize(Unused1);
	stream.Serialize(WalkRate);
	stream.Serialize(VeteranAbilities);
	stream.Serialize(EliteAbilities);
	stream.Serialize(SpecialThreatValue);
	stream.Serialize(MyEffectivenessCoefficient);
	stream.Serialize(TargetEffectivenessCoefficient);
	stream.Serialize(TargetSpecialThreatCoefficient);
	stream.Serialize(TargetStrengthCoefficient);
	stream.Serialize(TargetDistanceCoefficient);
	stream.Serialize(ThreatAvoidanceCoefficient);
	stream.Serialize(SlowdownDistance);
	stream.Serialize(DeaccelerationFactor);
	stream.Serialize(AccelerationFactor);
	stream.Serialize(CloakingSpeed);
	stream.Serialize(DebrisTypes);
	stream.Serialize(DebrisMaximums);
	stream.Serialize(DebrisAnims);
	stream.Serialize(DestroyAnim);

	/*
	 * A class identifier is a plain sixteen byte value from the Windows SDK with no member
	 * of its own to describe, so it travels as its raw image.
	 */
	stream.Serialize_Bytes(&Locomotor, sizeof(Locomotor));

	stream.Serialize(VoxelCenterY);
	stream.Serialize(VoxelCenterX);
	stream.Serialize(Weight);
	stream.Serialize(PhysicalSize);
	stream.Serialize(InitialMission);
	stream.Serialize(RollAngle);
	stream.Serialize(PitchSpeed);
	stream.Serialize(PitchAngle);
	stream.Serialize(BuildLimit);
	stream.Serialize(Category);
	stream.Serialize(Unused2);
	stream.Serialize(DeployTime);
	stream.Serialize(FireAngle);
	stream.Serialize(PipScale);
	stream.Serialize(MaxPips);
	stream.Serialize(Dock);
	stream.Serialize(DeploysInto);
	stream.Serialize(UndeploysInto);
	stream.Serialize(UnloadingClass);
	stream.Serialize(VoiceSelect);
	stream.Serialize(VoiceMove);
	stream.Serialize(VoiceAttack);
	stream.Serialize(VoiceDie);
	stream.Serialize(DieSound);
	stream.Serialize(MoveSound);
	stream.Serialize(VoiceFeedback);
	stream.Serialize(AuxSound1);
	stream.Serialize(AuxSound2);
	stream.Serialize(MZone);
	stream.Serialize(ThreatRange);
	stream.Serialize(MaxDebris);
	stream.Serialize(MinDebris);
	stream.Serialize(PixelSelectionBracketDelta);
	stream.Serialize(LeadershipRating);
	stream.Serialize(IsOrganic);
	stream.Serialize(IsDamageSelf);
	stream.Serialize(IsImmuneToPsionics);
	stream.Serialize(IsBerserkFriendly);
	stream.Serialize(IsImmuneToPsionicWeapons);
	stream.Serialize(IsImmuneToPoison);
	stream.Serialize(IsCanPassiveAquire);
	stream.Serialize(IsCanRetaliate);
	stream.Serialize(IsOpportunityFire);
	stream.Serialize(IsWarpable);
	stream.Serialize(IsImmuneToRadiation);
	stream.Serialize(GapRadiusInCells);
	stream.Serialize(BombSight);
	stream.Serialize(DeathWeapon);
	stream.Serialize(DeathWeaponDamageModifier);
	stream.Serialize(InitialAmmo);
	stream.Serialize(IsParasiteable);
	stream.Serialize(SuppressionThreshold);
	stream.Serialize(IsReselectIfLimboed);
	stream.Serialize(IsCanDisguise);
	stream.Serialize(IsPermaDisguise);
	stream.Serialize(IsBalloonHover);
	stream.Serialize(MindControlRingOffset);
	stream.Serialize(MindClearedSound);
	stream.Serialize(LeptonMindControlOffset);
	stream.Serialize(IsTeleporter);
	stream.Serialize(IsChronoshiftAllowed);
	stream.Serialize(IsChronoshiftCrushable);
	stream.Serialize(IsBounty);
	stream.Serialize(BountyDisplay);
	stream.Serialize(BountyValue);
	stream.Serialize(InsigniaFile);
	stream.Serialize(InsigniaFrame);
	stream.Serialize(InsigniaShowEnemy);
	stream.Serialize(IsVehicleThiefAllowed);
	stream.Serialize(BuildTimeMultipleFactory);
	stream.Serialize(IsCrashable);
	stream.Serialize(PromoteVeteranSound);
	stream.Serialize(PromoteEliteSound);
	stream.Serialize(IsHealthBarHidden);
	stream.Serialize(EMPModifier);
	stream.Serialize(EMPThreshold);
	stream.Serialize(AttachEffect);
	stream.Serialize(IsCanBeReversed);
	stream.Serialize(ReversedAs);
	stream.Serialize(GroupAs);
	stream.Serialize(KeepAlive);
	stream.Serialize(IsAttackFriendlies);
	stream.Serialize(IsAttackCursorOnFriendlies);
	stream.Serialize(IsDefaultToGuardArea);
	stream.Serialize(IsSelectableCombatant);
	stream.Serialize(BuildTimeMultiplier);
	stream.Serialize(ChronoInSound);
	stream.Serialize(ChronoOutSound);
	stream.Serialize(CreateSound);
	stream.Serialize(EnterTransportSound);
	stream.Serialize(LeaveTransportSound);
	stream.Serialize(VoiceEnter);
	stream.Serialize(VoiceDeploy);
	stream.Serialize(VoiceUndeploy);
	stream.Serialize(TurretRotateSound);
	stream.Serialize(IsResourceGatherer);
	stream.Serialize(IsResourceDestination);
	stream.Serialize(VoiceCapture);
	stream.Serialize(VoiceHarvest);
	stream.Serialize(ImpactLandSound);
	stream.Serialize(ImpactWaterSound);
	stream.Serialize(DamageSound);
	stream.Serialize(IsNatural);
	stream.Serialize(IsUnnatural);
	stream.Serialize(IsUnderwater);
	stream.Serialize(NavalTargeting);
	stream.Serialize(LandTargeting);
	stream.Serialize(VoicePrimaryWeaponAttack);
	stream.Serialize(VoicePrimaryEliteWeaponAttack);
	stream.Serialize(VoiceSecondaryWeaponAttack);
	stream.Serialize(VoiceSecondaryEliteWeaponAttack);
	stream.Serialize(Spawns);
	stream.Serialize(SpawnsNumber);
	stream.Serialize(SpawnRegenRate);
	stream.Serialize(SpawnReloadRate);
	stream.Serialize(SecondSpawnOffset);
	stream.Serialize(IsSpawned);
	stream.Serialize(IsMissileSpawn);
	stream.Serialize(IsDrainable);
	stream.Serialize(Enslaves);
	stream.Serialize(SlavesNumber);
	stream.Serialize(SlaveRegenRate);
	stream.Serialize(SlaveReloadRate);
	stream.Serialize(IsBunkerable);
	stream.Serialize(IsDistributedFire);
	stream.Serialize(IsCanApproachTarget);
	stream.Serialize(SensorsSight);
	stream.Serialize(IsPoweredUnit);
	stream.Serialize(ActivateSound);
	stream.Serialize(DeactivateSound);
	stream.Serialize(PowersUnit);
	stream.Serialize(CrashingSound);
	stream.Serialize(VoiceCrashing);
	stream.Serialize(SinkingSound);
	stream.Serialize(VoiceSinking);
	stream.Serialize(IsJumpjetDataSet);
	stream.Serialize(JumpjetTurnRate);
	stream.Serialize(JumpjetSpeed);
	stream.Serialize(JumpjetClimb);
	stream.Serialize(JumpjetHeight);
	stream.Serialize(JumpjetAccel);
	stream.Serialize(JumpjetWobbles);
	stream.Serialize(JumpjetDeviation);
	stream.Serialize(IsJumpjetNoWobbles);
	stream.Serialize(IsRevealToAll);
	stream.Serialize(AirstrikeTeam);
	stream.Serialize(EliteAirstrikeTeam);
	stream.Serialize(AirstrikeTeamType);
	stream.Serialize(EliteAirstrikeTeamType);
	stream.Serialize(AirstrikeRechargeTime);
	stream.Serialize(EliteAirstrikeRechargeTime);
	stream.Serialize(TurretCount);
	stream.Serialize(WeaponCount);
	stream.Serialize(IsGattling);
	stream.Serialize(WeaponStages);
	stream.Serialize(WeaponStage);
	stream.Serialize(EliteStage);
	stream.Serialize(RateUp);
	stream.Serialize(RateDown);
	stream.Serialize(IsGunner);
	stream.Serialize(IFVMode);
	stream.Serialize(AirRangeBonus);
	stream.Serialize(TurretWeapon);
	stream.Serialize(IsOpenTopped);
	stream.Serialize(OpenTransportWeapon);
	stream.Serialize(MaxPassengers);
	stream.Serialize(Size);
	stream.Serialize(SizeLimit);
	stream.Serialize(IsVehicleTransport);
	stream.Serialize(SightRange);
	stream.Serialize(Cost);
	stream.Serialize(Soylent);
	stream.Serialize(FlightLevel);
	stream.Serialize(Level);
	stream.Serialize(Prerequisite);
	stream.Serialize(Risk);
	stream.Serialize(Reward);
	stream.Serialize(MaxSpeed);
	stream.Serialize(Speed);
	stream.Serialize(MaxAmmo);
	stream.Serialize(Ownable);
	stream.Serialize(IsAllowedToStartInMultiplayer);
	stream.Serialize(CameoFilename);
	// CameoData -- artwork, fetched from the mix files again as this loads.
	stream.Serialize(CameoSortOrder);
	stream.Serialize(Rotation);
	stream.Serialize(ROT);
	stream.Serialize(TurretOffset);
	stream.Serialize(Points);
	stream.Serialize(Explosion);
	stream.Serialize(ScrapExplosion);
	stream.Serialize(NaturalParticleSystem);
	stream.Serialize(NaturalParticleLocation);
	stream.Serialize(DamageParticleSystems);
	stream.Serialize(DamageSmokeOffset);
	stream.Serialize(ShadowIndex);
	stream.Serialize(Capacity);
	stream.Serialize(TurretNotExportedOnGround);
	stream.Serialize(Weapons);
	stream.Serialize(EliteWeapons);
	stream.Serialize(IsTypeImmune);
	stream.Serialize(IsCanBeHidden);
	stream.Serialize(IsDetectDisguise);
	stream.Serialize(DetectDisguiseRange);
	stream.Serialize(IsMoveToShroud);
	stream.Serialize(IsTrainable);
	stream.Serialize(IsNaval);
	stream.Serialize(RequiredHouses);
	stream.Serialize(ForbiddenHouses);
	stream.Serialize(AIBasePlanningSide);
	stream.Serialize(IsRequiresStolenAlliedTech);
	stream.Serialize(IsRequiresStolenSovietTech);
	stream.Serialize(IsRequiresStolenThirdTech);
	stream.Serialize(IsDamageSparks);
	stream.Serialize(IsTargetLaser);
	stream.Serialize(IsImmuneToVeins);
	stream.Serialize(IsImmuneToEMP);
	stream.Serialize(IsTiberiumHeal);
	stream.Serialize(IsCloakStop);
	stream.Serialize(IsTrain);
	stream.Serialize(IsDropship);
	stream.Serialize(IsToProtect);
	stream.Serialize(IsDisableable);
	stream.Serialize(IsUnbuildable);
	stream.Serialize(IsDoubleOwned);
	stream.Serialize(IsInvisible);
	stream.Serialize(IsRadarVisible);
	stream.Serialize(IsLeader);
	stream.Serialize(IsScanner);
	stream.Serialize(IsNominal);
	stream.Serialize(IsTurretEquipped);
	stream.Serialize(IsRepairable);
	stream.Serialize(IsCrew);
	stream.Serialize(IsRemappable);
	stream.Serialize(IsCloakable);
	stream.Serialize(IsSelfHealing);
	stream.Serialize(SelfHealingStep);
	stream.Serialize(SelfHealingRate);
	stream.Serialize(SelfHealingCap);
	stream.Serialize(IsMechanic);
	stream.Serialize(IsOmniHealer);
	stream.Serialize(IsExploding);
	stream.Serialize(IsNoAutoFire);
	stream.Serialize(IsRadarEquipped);
	stream.Serialize(IsRegulated);
	stream.Serialize(IsManualReload);
	stream.Serialize(IsVisibleLoad);
	stream.Serialize(IsLightningRod);
	stream.Serialize(IsHunterSeeker);
	stream.Serialize(IsCrusher);
	stream.Serialize(IsTiltsWhenCrushes);
	stream.Serialize(IsSubterranean);
	stream.Serialize(IsAutoCrush);
	stream.Serialize(IsAccelerates);
	stream.Serialize(ZFudgeCliff);
	stream.Serialize(ZFudgeColumn);
	stream.Serialize(ZFudgeTunnel);
	stream.Serialize(ZFudgeBridge);
}


/// <summary>
/// Adds this object type's rule data to a running checksum.
/// This routine is used by the multiplayer sync check to prove that every machine in the
/// game is playing by the same rules.
/// </summary>
void TechnoTypeClass::Compute_CRC(class CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(ThreatAvoidanceCoefficient);
	crc(SlowdownDistance);
	crc(DeaccelerationFactor);
	crc(AccelerationFactor);
	crc(CloakingSpeed);
	crc(DebrisTypes.Count());
	AttachEffect.Compute_CRC(crc);
	crc(Dock.Count());
	crc(DebrisMaximums.Count());
	crc((char*)&Locomotor, sizeof(Locomotor));
	crc(VoxelCenterY);
	crc(VoxelCenterX);
	crc(Weight);
	crc(PhysicalSize);
	crc(InitialMission);
	crc(IsImmuneToVeins);
	crc(Is_Immune_To_EMP());
	crc(IsTiberiumHeal);
	crc(IsTargetLaser);
	crc(RollAngle);
	crc(PitchAngle);
	crc(PitchSpeed);
	crc(FlightLevel);
	crc(BuildLimit);
	crc(Category);
	crc(DeployTime);
	crc(FireAngle);
	crc(PipScale);
	crc(Max_Pips());
	crc(VoiceSelect.Count());
	crc(VoiceMove.Count());
	crc(VoiceAttack.Count());
	crc(VoiceDie.Count());
	crc(VoiceFeedback.Count());
	crc(DamageParticleSystems.Count());
	crc(MZone);
	crc(ThreatRange);
	crc(MaxDebris);
	crc(MaxPassengers);
	crc(Size);
	crc(SizeLimit);
	crc(IsVehicleTransport);
	crc(SightRange);
	crc(Cost);
	crc(Soylent);
	crc(Level);
	crc(Prerequisite.Count());
	crc(Risk);
	crc(Reward);
	crc(MaxSpeed);
	crc(Speed);
	crc(MaxAmmo);
	crc((int)Ownable);
	crc(Rotation);
	crc(ROT);
	crc(TurretOffset);
	crc(Points);
	crc(Explosion.Count());
	crc(ScrapExplosion.Count());
	crc(ShadowIndex);
	crc(Capacity);
	crc(IsTrain);
	crc(IsDetectDisguise);
	crc(IsDropship);
	crc(IsToProtect);
	crc(IsDisableable);
	crc(IsUnbuildable);
	crc(IsDoubleOwned);
	crc(IsInvisible);
	crc(IsRadarVisible);
	crc(IsLeader);
	crc(IsScanner);
	crc(IsNominal);
	crc(IsTurretEquipped);
	crc(IsRepairable);
	crc(IsCrew);
	crc(IsRemappable);
	crc(IsCloakable);
	crc(IsSelfHealing);
	crc(SelfHealingStep);
	crc(SelfHealingRate);
	crc(SelfHealingCap);
	crc(IsMechanic);
	crc(IsOmniHealer);
	crc(IsExploding);
	crc(IsNoAutoFire);
	crc(IsRadarEquipped);
	crc(IsRegulated);
	crc(IsManualReload);
	crc(IsVisibleLoad);
	crc(IsLightningRod);
}


/// <summary>
/// Fetches the weapon data for one of this object type's weapon slots.
/// </summary>
/// <param name="which">The weapon slot desired, below WEAPON_SLOT_COUNT.</param>
/// <returns>Returns with a pointer to the weapon data for that slot.</returns>
WeaponDataStruct const * TechnoTypeClass::Get_Weapon(int which) const
{
	return(&Weapons[which]);
}


/// <summary>
/// Fetches the elite weapon data for one of this object type's weapon slots.
/// An empty elite slot serves up the matching normal weapon instead, so an elite object may
/// ask for its elite armament without checking first.
/// </summary>
/// <param name="which">The weapon slot desired, below WEAPON_SLOT_COUNT.</param>
/// <returns>Returns with a pointer to the weapon data for that slot.</returns>
WeaponDataStruct const * TechnoTypeClass::Get_Elite_Weapon(int which) const
{
	if (EliteWeapons[which].Weapon == NULL) {
		return(&Weapons[which]);
	}
	return(&EliteWeapons[which]);
}


/// <summary>
/// Fetches the altitude that this object type travels at.
/// An object type that does not name a flight level of its own falls back on the global
/// rule, so most flying objects share one cruising height.
/// </summary>
/// <returns>Returns with the height above ground that this object type should fly at.</returns>
int TechnoTypeClass::Flight_Level(void) const
{
	if (FlightLevel == -1) {
		return(Rule->FlightLevel);
	}
	return(FlightLevel);
}
