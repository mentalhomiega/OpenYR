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
#include "dbgprint.h"
#include "findmake.h"
#include "globals.h"
#include "houstype.h"
#include "infatype.h"
#include "mixfile.h"
#include "objectkeyrules.h"
#include "psystype.h"
#include "rules.h"
#include "savestream.h"
#include "scenario.h"
#include "session.h"
#include "strutil.h"
#include "_tactica.h"
#include "tactical.h"
#include "tracker.h"
#include "unittype.h"
#include "vanimtype.h"
#include "warhead.h"
#include "warheadrules.h"
#include "weapon.h"

#include "pip.hh"
#include "voc.hh"

#include <algorithm>
#include <climits>
#include <cmath>
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
bool TechnoTypeClass::RadarJammersExist = false;


TechnoTypeClass::TechnoTypeClass(char const * ininame, SpeedType speed) :
	BASECLASS(ininame),
	IsDoubleOwned(false),
	IsInvisible(false),
	IsLeader(false),
	IsScanner(false),
	IsNominal(false),
	IsTurretEquipped(false),
	IsHasTurretTooltips(false),
	IsTurretRecoil(false),
	IsCrew(false),
	IsRepairable(true),
	IsRemappable(false),
	IsCloakable(false),
	IsCloakAllowed(true),
	IsNoShadow(false),
	IsDisableVoxelCache(false),
	IsDisableShadowCache(false),
	IsSelfHealing(false),
	SelfHealingStep(-1),
	SelfHealingRate(-1),
	SelfHealingCap(-1),
	SelfHealingCombatDelay(0),
	IsMechanic(false),
	IsOmniHealer(false),
	IsExploding(false),
	MZone(MZONE_NORMAL),
	ThreatRange(0),
	MaxPassengers(0),
	Size(1),
	SizeLimit(1),
	IsVehicleTransport(true),
	SightRange(0),
	DesignatorRange(-1),
	InhibitorRange(-1),
	Cost(0),
	Soylent(0),
	Level(255),
	Prerequisite(),
	PrerequisiteOverride(),
	Risk(0),
	Reward(0),
	MaxSpeed(MPH_IMMOBILE),
	Speed(speed),
	MaxAmmo(-1),
	ReloadDelay(0),
	EmptyReloadDelay(-1),
	ReloadIncrement(0),
	ReloadAmount(1),
	EmptyReloadAmountSetting(INT_MIN),
	PipWrap(0),
	Ownable(0),
	CameoData(NULL),
	AltCameoData(NULL),
	CameoSortOrder(0),
	Rotation(0),
	ROT(0),
	TurretROT(-1),
	Points(0),
	CollateralDamageCoefficient(.33f),
	Unused1(0.0f),
	WalkRate(1),
	IdleRate(0),
	AirRate(0),
	RadarJamRadius(0),
	IsPipsDrawForAll(false),
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
	VoiceSelectEnslaved(),
	VoiceSelectDeactivated(),
	VoiceMove(),
	VoiceSpecialAttack(),
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
	IsHoverAttack(false),
	IsJumpJet(false),
	IsTiltCrashJumpjet(false),
	VHPScan(VHP_SCAN_NONE),
	IsOpportunityFire(false),
	IsWarpable(true),
	IsImmuneToRadiation(false),
	IsImmuneToAbduction(false),
	IsProtectedDriver(false),
	ProtectedDriverMinHealth(1.0),
	IronCurtainModifier(1.0),
	ForceShieldModifier(1.0),
	IsGapGenerator(false),
	GapRadiusInCells(0),
	SuperGapRadiusInCells(0),
	IsDeployFire(false),
	DeployFireWeapon(1),
	IsDeployToLand(false),
	IsDisguiseWhenStill(false),
	IsSlaved(false),
	UndeployDelay(-1),
	BombSight(0),
	DeathWeapon(NULL),
	DeathWeaponDamageModifier(1.0),
	CrushDamage{0, 0, 0},
	CrushDamageWarhead(NULL),
	InitialAmmo(-1),
	IsParasiteable(true),
	SuppressionThreshold(0),
	IsReselectIfLimboed(false),
	IsRejoinTeamIfLimboed(false),
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
	IsCanBeDriven(true),
	BuildTimeMultipleFactory(-1.0),
	IsCrashable(false),
	IsConsideredAircraft(false),
	PromoteVeteranSound(VOC_NONE),
	PromoteEliteSound(VOC_NONE),
	IsVoiceAirstrikeAttackSet(false),
	VoiceAirstrikeAttack(VOC_NONE),
	PromoteVeteranFlash(-1),
	PromoteEliteFlash(-1),
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
	SurvivorSide{},
	CrewTechnicianChance(-1),
	CrewEngineerChance(-1),
	SurvivorPilots(-1),
	SurvivorPilotChance{-1, -1, -1},
	SurvivorPassengerChance{-1, -1, -1},
	ParachuteAnim(NULL),
	SlamSound(VOC_NONE),
	IsSlamSoundSet(false),
	RadialIndicatorRadius(0.0),
	WaterImage(NULL),
	IsCloneable(true),
	ClonedAt(),
	FactoryOwners(),
	FactoryOwnersForbidden(),
	CursorMove(),
	CursorNoMove(),
	CursorDeploy(),
	CursorNoDeploy(),
	CursorEnter(),
	CursorNoEnter(),
	CloakSound(VOC_NONE),
	DecloakSound(VOC_NONE),
	IsCloakSoundSet(false),
	IsDecloakSoundSet(false),
	CloakStages(0),
	IsCanPassiveAquireGuard(true),
	IsCanPassiveAquireCloak(true),
	IsNoManualFire(false),
	IsGattlingCycle(false),
	EvaPromoted{},
	EvaLost(),
	IsEvaLostSet(false),
	ChronoInSound(VOC_NONE),
	ChronoOutSound(VOC_NONE),
	CreateSound(VOC_NONE),
	EnterTransportSound(VOC_NONE),
	LeaveTransportSound(VOC_NONE),
	LeaveBioReactorSound(VOC_NONE),
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
	IsCloseRange(false),
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
	IsDrainLocal(false),
	DrainAmount(0),
	Enslaves(NULL),
	SlavesNumber(0),
	SlaveRegenRate(0),
	SlaveReloadRate(0),
	IsBunkerable(false),
	IsDistributedFire(false),
	IsCanApproachTarget(true),
	IsCanRecalcApproachTarget(true),
	IsStupidHunt(false),
	SensorsSight(0),
	IsPoweredUnit(false),
	ActivateSound(VOC_NONE),
	DeactivateSound(VOC_NONE),
	PowersUnit(NULL),
	IsOperatorAny(false),
	CrashingSound(VOC_NONE),
	VoiceCrashing(VOC_NONE),
	SinkingSound(VOC_NONE),
	VoiceSinking(VOC_NONE),
	IsJumpjetDataSet(false),
	JumpjetTurnRate(4),
	JumpjetSpeed(14),
	JumpjetClimb(5),
	JumpjetCrash(5),
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
	IsClearAllWeapons(false),
	IsMobileFire(true),
	IsPreventAttackMove(false),
	IsGattling(false),
	IsChargeTurret(false),
	IsPassengerTurret(false),
	WeaponStages(0),
	WeaponStage{},
	EliteStage{},
	RateUp(0),
	RateDown(0),
	IsGunner(false),
	IFVMode(0),
	AirRangeBonus(0),
	RadialFireSegments(0),
	IsOpenTopped(false),
	OpenTransportWeapon(-1),
	FlightLevel(-1),
	IsAllowedToStartInMultiplayer(true),
	CameoFilename(""),
	TurretOffset(0),
	Explosion(),
	ScrapExplosion(),
	NaturalParticleSystem(NULL),
	RefinerySmokeParticleSystem(NULL),
	RefinerySmokeOffset{TPoint3D<int>(0,0,0), TPoint3D<int>(0,0,0), TPoint3D<int>(0,0,0), TPoint3D<int>(0,0,0)},
	NaturalParticleLocation(0,0,0),
	DamageParticleSystems(),
	DamageSmokeOffset(0,0,0),
	DamSmkOffScrnRel(false),
	DestroyParticleSystems(),
	DestroySmokeOffset(0,0,0),
	ShadowIndex(0),
	Capacity(0),
	TurretNotExportedOnGround(false),
	IsTypeImmune(false),
	IsDamageReducesReadiness(false),
	IsSprayAttack(false),
	ReadinessReductionMultiplier(0.0f),
	IsCanBeHidden(true),
	IsDetectDisguise(false),
	DetectDisguiseRange(0),
	IsMoveToShroud(true),
	IsTrainable(true),
	IsDontScore(false),
	IsNaval(false),
	RequiredHouses(-1),
	ForbiddenHouses(-1),
	AIBasePlanningSide(-1),
	IsRequiresStolenAlliedTech(false),
	IsRequiresStolenSovietTech(false),
	IsRequiresStolenThirdTech(false),
	IsDamageSparks(false),
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
	IsHunterSeekerIgnored(false),
	IsCrusher(false),
	IsPushy(false),
	IsOmniCrusher(false),
	IsOmniCrushResistant(false),
	IsTiltsWhenCrushes(true),
	IsSubterranean(false),
	IsAutoCrush(false),
	IsAccelerates(true),
	ZFudgeCliff(10),
	ZFudgeColumn(5),
	ZFudgeTunnel(10),
	ZFudgeBridge(0),
	NoAmmoWeapon(-1),
	NoAmmoAmount(0),
	IsNoManualEnter(false),
	IsNoManualUnload(false),
	IsPassengersBySize(true),
	FallRateParachute(INT_MIN),
	FallRateNoParachute(INT_MIN),
	FallRateParachuteMax(INT_MIN),
	FallRateNoParachuteMax(INT_MIN),
	IsCivilianEnemy(false),
	IsOmniCrusherAggressive(true),
	IsExperienceFromPassengers(true),
	IsExperiencePromotePassengers(false),
	ExperiencePassengerModifier(1.0),
	IsExperienceFromAirstrike(false),
	ExperienceAirstrikeModifier(1.0),
	ExperienceMindControlSelfModifier(0.0),
	ExperienceMindControlVictimModifier(1.0),
	ExperienceSpawnOwnerModifier(0.0),
	ExperienceSpawnModifier(1.0),
	IsLowSelectionPriority(false)
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

	AlternativeFLH.assign(5, TPoint3D<int>(0, 0, 0));

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
		return((int)(Raw_Cost() * house->Cost_Multiplier(this)));
	}
	return(Raw_Cost());
}


/**********************************************************************************************
 * TechnoTypeClass::Refund -- Fetches the credits a sale of this type refunds to a house.     *
 *                                                                                            *
 *    This follows TechnoTypeClass::GetRefund (0x711F60). A Soylent value replaces the price, *
 *    scaled only by the country's multiplier for the type's category. Otherwise the price is *
 *    the raw cost (the reduced cost of a structure with a free unit) times that multiplier   *
 *    and the factory plant bonus, without the difficulty's or the country's Cost=. A human   *
 *    player's house then keeps RefundPercent of the price, truncated again. An unscaled      *
 *    refund ignores RefundPercent, as the compensation for a free unit that cannot be placed *
 *    does.                                                                                   *
 *                                                                                            *
 * INPUT:   house     -- The house receiving the refund; NULL refunds the raw cost.           *
 *          unscaled  -- Ignore RefundPercent.                                                *
 *                                                                                            *
 * OUTPUT:  Returns with the credits to refund.                                               *
 *                                                                                            *
 * WARNINGS:   none                                                                           *
*=============================================================================================*/
int TechnoTypeClass::Refund(HouseClass const * house, bool unscaled) const
{
	double const percent = unscaled ? 1.0 : static_cast<float>(Rule->RefundPercent);
	if (house == NULL) {
		return(static_cast<int>(Raw_Cost() * percent));
	}

	double const country = static_cast<float>(house->Country_Cost_Multiplier(this));
	if (Soylent != 0) {
		return(static_cast<int>(Soylent * country));
	}

	double const price = Raw_Cost() * static_cast<float>(house->Plant_Cost_Multiplier(this)) * country;
	if (house->Is_Human_Player()) {
		return(static_cast<int>(static_cast<int>(price) * percent));
	}
	return(static_cast<int>(price));
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
	if (AltCameoData != NULL && Is_Alt_Cameo_Shown()) {
		return(AltCameoData);
	}
	return(CameoData);
}


/// <summary>
/// The PCX picture to draw as this type's sidebar cameo, or an empty name when the cameo shape is drawn. The alternate
/// picture replaces the plain one while the alternate cameo is shown, if the type has one.
/// </summary>
std::string const & TechnoTypeClass::Get_Cameo_PCX(void) const
{
	if (!AltCameoPCX.empty() && Is_Alt_Cameo_Shown()) {
		return(AltCameoPCX);
	}
	return(CameoPCX);
}


// Once the local player has spied on a barracks or war factory, the trainable types it can make show their alternate cameo.
bool TechnoTypeClass::Is_Alt_Cameo_Shown(void) const
{
	if (PlayerPtr != NULL && IsTrainable) {
		RTTIType const rtti = Fetch_RTTI();
		bool const barracks = PlayerPtr->IsBarracksInfiltrated && rtti == RTTI_INFANTRYTYPE;
		bool const factory = PlayerPtr->IsWarFactoryInfiltrated && (rtti == RTTI_UNITTYPE || (rtti == RTTI_BUILDINGTYPE && UndeploysInto != NULL));
		return(barracks || factory);
	}
	// A country's VeteranBuildings show their alternate cameo whether or not they are trainable (Ares).
	if (PlayerPtr != NULL && PlayerPtr->Class != NULL && Fetch_RTTI() == RTTI_BUILDINGTYPE && PlayerPtr->Class->VeteranBuildings.Is_In_List((BuildingTypeClass const *)this)) {
		return(true);
	}
	return(false);
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
/// <param name="rank">0 rookie, 1 veteran, 2 elite.</param>
/// <returns>Returns with the Ares SelfHealing.Amount, else its rank form (either may be zero), else the
/// type's step, else the game-wide one, never below one.</returns>
int TechnoTypeClass::Self_Heal_Step(int rank) const
{
	if (SelfHealingAmount[0] >= 0) {
		return(SelfHealingAmount[0]);
	}
	if (SelfHealingAmount[1 + std::clamp(rank, 0, 2)] >= 0) {
		return(SelfHealingAmount[1 + std::clamp(rank, 0, 2)]);
	}
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
/// <param name="rank">0 rookie, 1 veteran, 2 elite.</param>
/// <returns>Returns with the Ares SelfHealing.Max, else its rank form, else the type's ceiling, else the
/// game-wide one, else full strength.</returns>
double TechnoTypeClass::Self_Heal_Cap(int rank) const
{
	if (SelfHealingMax[0] >= 0) {
		return(SelfHealingMax[0]);
	}
	if (SelfHealingMax[1 + std::clamp(rank, 0, 2)] >= 0) {
		return(SelfHealingMax[1 + std::clamp(rank, 0, 2)]);
	}
	if (SelfHealingCap >= 0) {
		return(SelfHealingCap);
	}
	// TechnoClass::ShouldSelfHealOneStep (0x70BE80) heals up to full strength.
	return(Rule->SelfHealCap >= 0 ? Rule->SelfHealCap : 1.0);
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
/// Tests a country's Ownable bits against this type's RequiredHouses and ForbiddenHouses.
/// </summary>
/// <param name="mask">The country bits to test, in the Ownable layout.</param>
/// <returns>Returns false when RequiredHouses is set and shares no bit with the mask, or when
/// ForbiddenHouses is set and shares a bit with it. A value of -1 places no limit.</returns>
bool TechnoTypeClass::Allows_Country_Mask(int mask) const
{
	if (RequiredHouses != -1 && (RequiredHouses & mask) == 0) return(false);
	if (ForbiddenHouses != -1 && (ForbiddenHouses & mask) != 0) return(false);
	return(true);
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


// A recoil part keeps at least one frame in each phase, as gamemd's frame setters do (0x717A50, 0x717A80, 0x717AB0).
static void Keep_Recoil_Frames(int & frames)
{
	if (frames < 1) {
		frames = 1;
	}
}


// Maps an EMP.Threshold= word to its value, matched without case. Any other text is read as a number.
static int Emp_Threshold_Named(char const * name)
{
	struct ThresholdName {
		char const * Name;
		int Threshold;
	};
	static ThresholdName const NAMES[] = {
		{"inair", -1},
		{"yes", 1},
		{"true", 1},
		{"no", 0},
		{"false", 0},
	};
	for (ThresholdName const & entry : NAMES) {
		if (stricmp(name, entry.Name) == 0) {
			return(entry.Threshold);
		}
	}
	return(atoi(name));
}


// Maps a VHPScan= name to its scan type, matched without case. A name not listed returns the fallback.
static int VHPScan_Named(char const * name, int fallback)
{
	struct ScanName {
		char const * Name;
		VHPScanType Scan;
	};
	static ScanName const NAMES[] = {
		{"none", VHP_SCAN_NONE},
		{"normal", VHP_SCAN_NORMAL},
		{"strong", VHP_SCAN_STRONG},
	};
	for (ScanName const & entry : NAMES) {
		if (stricmp(name, entry.Name) == 0) {
			return(entry.Scan);
		}
	}
	return(fallback);
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
	// The art section the base class reads depends on this flag.
	IsAlternateTheaterArt = ini.Get_Bool(Name(), "AlternateTheaterArt", IsAlternateTheaterArt);

	if (BASECLASS::Read_INI(ini)) {
		// Keep this order: a type that a key names is created when it is first read, and the art
		// readers start from values the rules readers set.
		Read_General_INI(ini);
		Read_Movement_INI(ini);
		Read_Abilities_INI(ini);
		Read_Targeting_INI(ini);
		Read_Bounty_INI(ini);
		Read_Insignia_INI(ini);
		Read_Attach_Effect_INI(ini);
		Read_Spawns_INI(ini);
		Read_Jumpjet_INI(ini);
		Read_Airstrike_INI(ini);
		Read_Destruction_INI(ini);
		Read_Weapons_INI(ini);
		Read_Transport_INI(ini);
		Read_Gattling_INI(ini);
		Read_Deploy_INI(ini);
		Read_Turret_INI(ini);
		Read_Effects_INI(ini);
		Read_Ammo_INI(ini);
		Read_Production_INI(ini);
		Read_AI_INI(ini);
		Read_Sounds_INI(ini);
		Read_Art_INI();
		Read_Weapon_FLH_INI();

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


void TechnoTypeClass::Read_General_INI(CCINIClass const & ini)
{
	// CanBeHidden comes from the art file, under the type's own name (TechnoTypeClass::LoadFromINI).
	IsCanBeHidden = ArtINI.Get_Bool(Name(), "CanBeHidden", IsCanBeHidden);
	IsTypeImmune = ini.Get_Bool(Name(), "TypeImmune", IsTypeImmune);
	IsDamageReducesReadiness = ini.Get_Bool(Name(), "DamageReducesReadiness", IsDamageReducesReadiness);
	IsSprayAttack = ini.Get_Bool(Name(), "SprayAttack", IsSprayAttack);
	ReadinessReductionMultiplier = ini.Get_Float(Name(), "ReadinessReductionMultiplier", ReadinessReductionMultiplier);
	IsDetectDisguise = ini.Get_Bool(Name(), "DetectDisguise", IsDetectDisguise);
	DetectDisguiseRange = ini.Get_Int(Name(), "DetectDisguiseRange", DetectDisguiseRange);
	WalkRate = ini.Get_Int(Name(), "WalkRate", WalkRate);
	IdleRate = ini.Get_Int(Name(), "IdleRate", IdleRate);
	AirRate = ini.Get_Int(Name(), "AirRate", AirRate);
	RadarJamRadius = ini.Get_Int(Name(), "RadarJamRadius", RadarJamRadius);
	if (RadarJamRadius > 0) {
		RadarJammersExist = true;
	}

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
	CloakingSpeed = ini.Get_Int(Name(), "CloakingSpeed", CloakingSpeed);
	PixelSelectionBracketDelta = ini.Get_Int(Name(), "PixelSelectionBracketDelta", PixelSelectionBracketDelta);
	IsReselectIfLimboed = ini.Get_Bool(Name(), "ReselectIfLimboed", IsReselectIfLimboed);
	IsRejoinTeamIfLimboed = ini.Get_Bool(Name(), "RejoinTeamIfLimboed", IsRejoinTeamIfLimboed);
	MindControlRingOffset = ini.Get_Int(Name(), "MindControlRingOffset", MindControlRingOffset);
	LeptonMindControlOffset = ini.Get_Int(Name(), "LeptonMindControlOffset", LeptonMindControlOffset);
	IsHealthBarHidden = ini.Get_Bool(Name(), "HealthBar.Hide", IsHealthBarHidden);
	IsSelectableCombatant = ini.Get_Bool(Name(), "IsSelectableCombatant", IsSelectableCombatant);

	char value[64];
	if (ini.Get_String(Name(), "GroupAs", "", value, sizeof(value)) > 0) {
		GroupAs = value;
	}
	if (ini.Is_Present(Name(), "KeepAlive")) {
		KeepAlive = ini.Get_Bool(Name(), "KeepAlive", false) ? 1 : 0;
		Rule->IsKeepAliveSet = true;
	}

	IsResourceGatherer = ini.Get_Bool(Name(), "ResourceGatherer", IsResourceGatherer);
	IsResourceDestination = ini.Get_Bool(Name(), "ResourceDestination", IsResourceDestination);
	IsNatural = ini.Get_Bool(Name(), "Natural", IsNatural);
	IsUnnatural = ini.Get_Bool(Name(), "Unnatural", IsUnnatural);
	SensorsSight = ini.Get_Int(Name(), "SensorsSight", SensorsSight);
	IsRevealToAll = ini.Get_Bool(Name(), "RevealToAll", IsRevealToAll);
	IsCloakStop = ini.Get_Bool(Name(), "CloakStop", IsCloakStop);
	Capacity = ini.Get_Int(Name(), "Storage", Capacity);
	IsNominal = ini.Get_Bool(Name(), "Nominal", IsNominal);
	IsCloakable = ini.Get_Bool(Name(), "Cloakable", IsCloakable);
	IsCloakAllowed = ini.Get_Bool(Name(), "Cloakable.Allowed", IsCloakAllowed);
	IsNoShadow = ini.Get_Bool(Name(), "NoShadow", IsNoShadow);
	IsScanner = ini.Get_Bool(Name(), "Sensors", IsScanner);
	SightRange = ini.Get_Int(Name(), "Sight", SightRange);
	DesignatorRange = ini.Get_Int(Name(), "DesignatorRange", DesignatorRange);
	InhibitorRange = ini.Get_Int(Name(), "InhibitorRange", InhibitorRange);
	Reward = Points = ini.Get_Int(Name(), "Points", Points);
	IsDontScore = ini.Get_Bool(Name(), "DontScore", IsDontScore);
	IsInvisible = ini.Get_Bool(Name(), "Invisible", IsInvisible);
	IsRadarVisible = ini.Get_Bool(Name(), "RadarVisible", IsRadarVisible);
	IsHunterSeeker = ini.Get_Bool(Name(), "HunterSeeker", IsHunterSeeker);
	IsHunterSeekerIgnored = ini.Get_Bool(Name(), "HunterSeeker.Ignore", IsHunterSeekerIgnored);
	ZFudgeCliff = ini.Get_Int(Name(), "ZFudgeCliff", ZFudgeCliff);
	ZFudgeColumn = ini.Get_Int(Name(), "ZFudgeColumn", ZFudgeColumn);
	ZFudgeTunnel = ini.Get_Int(Name(), "ZFudgeTunnel", ZFudgeTunnel);
	ZFudgeBridge = ini.Get_Int(Name(), "ZFudgeBridge", ZFudgeBridge);
}


void TechnoTypeClass::Read_Movement_INI(CCINIClass const & ini)
{
	IsMoveToShroud = ini.Get_Bool(Name(), "MoveToShroud", IsMoveToShroud);
	IsTrain = ini.Get_Bool(Name(), "IsTrain", IsTrain);
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
	ThreatAvoidanceCoefficient = ini.Get_Float(Name(), "ThreatAvoidanceCoefficient", ThreatAvoidanceCoefficient);
	SlowdownDistance = ini.Get_Int(Name(), "SlowdownDistance", SlowdownDistance);
	DeaccelerationFactor = ini.Get_Float(Name(), "DeaccelerationFactor", DeaccelerationFactor);
	AccelerationFactor = ini.Get_Float(Name(), "AccelerationFactor", AccelerationFactor);
	Weight = ini.Get_Float(Name(), "Weight", Weight);
	PhysicalSize = ini.Get_Float(Name(), "PhysicalSize", PhysicalSize);
	IsJumpJet = ini.Get_Bool(Name(), "JumpJet", IsJumpJet);
	IsTiltCrashJumpjet = ini.Get_Bool(Name(), "TiltCrashJumpjet", IsTiltCrashJumpjet);
	IsBalloonHover = ini.Get_Bool(Name(), "BalloonHover", IsBalloonHover);
	IsUnderwater = ini.Get_Bool(Name(), "Underwater", IsUnderwater);
	int maxspeed = ini.Get_Int(Name(), "Speed", -1);
	if (maxspeed != -1) {
		MaxSpeed = MPHType(_Scale_To_256(maxspeed));
	}

	IsNaval = ini.Get_Bool(Name(), "Naval", IsNaval);
	IsConsideredAircraft = ini.Get_Bool(Name(), "ConsideredAircraft", IsConsideredAircraft);
	ROT = ini.Get_Int(Name(), "ROT", ROT);
	TurretROT = ini.Get_Int(Name(), "TurretROT", TurretROT);
	IsCrusher = ini.Get_Bool(Name(), "Crusher", IsCrusher);
	IsPushy = ini.Get_Bool(Name(), "Pushy", IsPushy);
	IsOmniCrusher = ini.Get_Bool(Name(), "OmniCrusher", IsOmniCrusher);
	IsOmniCrusherAggressive = ini.Get_Bool(Name(), "OmniCrusher.Aggressive", IsOmniCrusherAggressive);
	NoAmmoWeapon = ini.Get_Int(Name(), "NoAmmoWeapon", NoAmmoWeapon);
	NoAmmoAmount = ini.Get_Int(Name(), "NoAmmoAmount", NoAmmoAmount);
	IsNoManualEnter = ini.Get_Bool(Name(), "NoManualEnter", IsNoManualEnter);
	IsNoManualUnload = ini.Get_Bool(Name(), "NoManualUnload", IsNoManualUnload);
	IsCivilianEnemy = ini.Get_Bool(Name(), "CivilianEnemy", IsCivilianEnemy);
	IsLowSelectionPriority = ini.Get_Bool(Name(), "LowSelectionPriority", IsLowSelectionPriority);
	IsPassengersBySize = ini.Get_Bool(Name(), "Passengers.BySize", IsPassengersBySize);
	if (ini.Is_Present(Name(), "Passengers.Allowed")) {
		PassengersAllowed = ObjectKeyRules::Split_List(ini.Get_String(Name(), "Passengers.Allowed"));
	}
	if (ini.Is_Present(Name(), "Passengers.Disallowed")) {
		PassengersDisallowed = ObjectKeyRules::Split_List(ini.Get_String(Name(), "Passengers.Disallowed"));
	}
	if (ini.Is_Present(Name(), "InitialPayload.Types")) {
		InitialPayloadTypes = ObjectKeyRules::Split_List(ini.Get_String(Name(), "InitialPayload.Types"));
	}
	if (ini.Is_Present(Name(), "InitialPayload.Nums")) {
		InitialPayloadNums.clear();
		for (std::string const & count : ObjectKeyRules::Split_List(ini.Get_String(Name(), "InitialPayload.Nums"))) {
			InitialPayloadNums.push_back(atoi(count.c_str()));
		}
	}
	FallRateParachute = ini.Get_Int(Name(), "FallRate.Parachute", FallRateParachute);
	FallRateNoParachute = ini.Get_Int(Name(), "FallRate.NoParachute", FallRateNoParachute);
	FallRateParachuteMax = ini.Get_Int(Name(), "FallRate.ParachuteMax", FallRateParachuteMax);
	FallRateNoParachuteMax = ini.Get_Int(Name(), "FallRate.NoParachuteMax", FallRateNoParachuteMax);
	IsExperienceFromPassengers = ini.Get_Bool(Name(), "Experience.FromPassengers", IsExperienceFromPassengers);
	IsExperiencePromotePassengers = ini.Get_Bool(Name(), "Experience.PromotePassengers", IsExperiencePromotePassengers);
	ExperiencePassengerModifier = ini.Get_Float(Name(), "Experience.PassengerModifier", ExperiencePassengerModifier);
	IsExperienceFromAirstrike = ini.Get_Bool(Name(), "Experience.FromAirstrike", IsExperienceFromAirstrike);
	ExperienceAirstrikeModifier = ini.Get_Float(Name(), "Experience.AirstrikeModifier", ExperienceAirstrikeModifier);
	ExperienceMindControlSelfModifier = ini.Get_Float(Name(), "Experience.MindControlSelfModifier", ExperienceMindControlSelfModifier);
	ExperienceMindControlVictimModifier = ini.Get_Float(Name(), "Experience.MindControlVictimModifier", ExperienceMindControlVictimModifier);
	ExperienceSpawnOwnerModifier = ini.Get_Float(Name(), "Experience.SpawnOwnerModifier", ExperienceSpawnOwnerModifier);
	ExperienceSpawnModifier = ini.Get_Float(Name(), "Experience.SpawnModifier", ExperienceSpawnModifier);
	if (ini.Is_Present(Name(), "FakeOf")) {
		FakeOf = ini.Get_String(Name(), "FakeOf");
	}
	if (ini.Is_Present(Name(), "EnemyUIName")) {
		EnemyUIName = ini.Get_String(Name(), "EnemyUIName");
		EnemyUINameText.clear();
	}
	IsOmniCrushResistant = ini.Get_Bool(Name(), "OmniCrushResistant", IsOmniCrushResistant);
	IsAutoCrush = ini.Get_Bool(Name(), "AutoCrush", IsAutoCrush);
	IsTiltsWhenCrushes = ini.Get_Bool(Name(), "TiltsWhenCrushes", IsTiltsWhenCrushes);
	IsAccelerates = ini.Get_Bool(Name(), "Accelerates", IsAccelerates);

	/*
	**	Check to see what zone this object should recognize.
	*/
	MZone = ini.Get_MZoneType(Name(), "MovementZone", MZone);
	Speed = ini.Get_SpeedType(Name(), "SpeedType", Speed);
	IsSubterranean = MZone == MZONE_SUBTERANNEAN;
}


void TechnoTypeClass::Read_Abilities_INI(CCINIClass const & ini)
{
	IsOrganic = ini.Get_Bool(Name(), "Organic", IsOrganic);
	IsDamageSelf = ini.Get_Bool(Name(), "DamageSelf", IsDamageSelf);
	IsImmuneToPsionics = ini.Get_Bool(Name(), "ImmuneToPsionics", IsImmuneToPsionics);
	IsBerserkFriendly = ini.Get_Bool(Name(), "BerserkFriendly", IsBerserkFriendly);
	IsImmuneToPsionicWeapons = ini.Get_Bool(Name(), "ImmuneToPsionicWeapons", IsImmuneToPsionicWeapons);
	IsImmuneToPoison = ini.Get_Bool(Name(), "ImmuneToPoison", IsImmuneToPoison);
	IsWarpable = ini.Get_Bool(Name(), "Warpable", IsWarpable);
	IsImmuneToRadiation = ini.Get_Bool(Name(), "ImmuneToRadiation", IsImmuneToRadiation);
	IronCurtainModifier = ini.Get_Float(Name(), "IronCurtain.Modifier", IronCurtainModifier);
	ForceShieldModifier = ini.Get_Float(Name(), "ForceShield.Modifier", ForceShieldModifier);
	IsImmuneToAbduction = ini.Get_Bool(Name(), "ImmuneToAbduction", IsImmuneToAbduction);
	IsProtectedDriver = ini.Get_Bool(Name(), "ProtectedDriver", IsProtectedDriver);
	ProtectedDriverMinHealth = WarheadRules::Protected_Driver_Min_Health(IsProtectedDriver, ini.Is_Present(Name(), "ProtectedDriver.MinHealth"), ini.Get_Float(Name(), "ProtectedDriver.MinHealth", 0.0));
	IsGapGenerator = ini.Get_Bool(Name(), "GapGenerator", IsGapGenerator);
	GapRadiusInCells = ini.Get_Int(Name(), "GapRadiusInCells", GapRadiusInCells);
	SuperGapRadiusInCells = ini.Get_Int(Name(), "SuperGapRadiusInCells", SuperGapRadiusInCells);
	IsDisguiseWhenStill = ini.Get_Bool(Name(), "DisguiseWhenStill", IsDisguiseWhenStill);
	DeathWeapon = TGet_Class(ini, Name(), "DeathWeapon", DeathWeapon);
	DeathWeaponDamageModifier = ini.Get_Float(Name(), "DeathWeaponDamageModifier", DeathWeaponDamageModifier);
	// CrushDamage sets all three ranks; a rank's own key in the same file then overrides it.
	if (ini.Is_Present(Name(), "CrushDamage")) {
		int const all = ini.Get_Int(Name(), "CrushDamage", 0);
		CrushDamage[0] = CrushDamage[1] = CrushDamage[2] = all;
	}
	CrushDamage[0] = ini.Get_Int(Name(), "CrushDamage.Rookie", CrushDamage[0]);
	CrushDamage[1] = ini.Get_Int(Name(), "CrushDamage.Veteran", CrushDamage[1]);
	CrushDamage[2] = ini.Get_Int(Name(), "CrushDamage.Elite", CrushDamage[2]);
	CrushDamageWarhead = TGet_Class(ini, Name(), "CrushDamage.Warhead", CrushDamageWarhead);
	IsParasiteable = ini.Get_Bool(Name(), "Parasiteable", IsParasiteable);
	SuppressionThreshold = ini.Get_Int(Name(), "SuppressionThreshold", SuppressionThreshold);
	IsCanDisguise = ini.Get_Bool(Name(), "CanDisguise", IsCanDisguise);
	IsPermaDisguise = ini.Get_Bool(Name(), "PermaDisguise", IsPermaDisguise);
	IsTeleporter = ini.Get_Bool(Name(), "Teleporter", IsTeleporter);
	IsChronoshiftAllowed = ini.Get_Bool(Name(), "Chronoshift.Allow", IsChronoshiftAllowed);
	IsChronoshiftCrushable = ini.Get_Bool(Name(), "Chronoshift.Crushable", IsChronoshiftCrushable);
	IsVehicleThiefAllowed = ini.Get_Bool(Name(), "VehicleThief.Allowed", IsVehicleThiefAllowed);
	IsCanBeDriven = ini.Get_Bool(Name(), "CanBeDriven", IsCanBeDriven);
	EMPModifier = ini.Get_Float(Name(), "EMP.Modifier", EMPModifier);
	IsCanBeReversed = ini.Get_Bool(Name(), "CanBeReversed", IsCanBeReversed);

	char value[64];
	if (ini.Get_String(Name(), "ReversedAs", "", value, sizeof(value)) > 0) {
		ReversedAs = value;
	}
	if (ini.Get_String(Name(), "EMP.Threshold", "", value, sizeof(value)) > 0) {
		EMPThreshold = Emp_Threshold_Named(value);
	}

	IsDrainable = ini.Get_Bool(Name(), "Drainable", IsDrainable);
	IsDrainLocal = ini.Get_Bool(Name(), "Drain.Local", IsDrainLocal);
	DrainAmount = ini.Get_Int(Name(), "Drain.Amount", DrainAmount);
	IsBunkerable = ini.Get_Bool(Name(), "Bunkerable", IsBunkerable);
	IsLightningRod = ini.Get_Bool(Name(), "LightningRod", IsLightningRod);
	IsTrainable = ini.Get_Bool(Name(), "Trainable", IsTrainable);
	IsCrew = ini.Get_Bool(Name(), "Crewed", IsCrew);
	IsRepairable = ini.Get_Bool(Name(), "Repairable", IsRepairable);
	IsSelfHealing = ini.Get_Bool(Name(), "SelfHealing", IsSelfHealing);
	SelfHealingStep = ini.Get_Int(Name(), "SelfHealingStep", SelfHealingStep);
	SelfHealingRate = ini.Get_Float(Name(), "SelfHealingRate", SelfHealingRate);
	SelfHealingRate = ini.Get_Float(Name(), "SelfHealing.Rate", SelfHealingRate);
	SelfHealingCap = ini.Get_Float(Name(), "SelfHealingCap", SelfHealingCap);
	SelfHealingAmount[0] = std::max(ini.Get_Int(Name(), "SelfHealing.Amount", SelfHealingAmount[0]), -1);
	SelfHealingAmount[1] = std::max(ini.Get_Int(Name(), "SelfHealing.RookieAmount", SelfHealingAmount[1]), -1);
	SelfHealingAmount[2] = std::max(ini.Get_Int(Name(), "SelfHealing.VeteranAmount", SelfHealingAmount[2]), -1);
	SelfHealingAmount[3] = std::max(ini.Get_Int(Name(), "SelfHealing.EliteAmount", SelfHealingAmount[3]), -1);
	SelfHealingMax[0] = ini.Get_Float(Name(), "SelfHealing.Max", SelfHealingMax[0]);
	SelfHealingMax[1] = ini.Get_Float(Name(), "SelfHealing.RookieMax", SelfHealingMax[1]);
	SelfHealingMax[2] = ini.Get_Float(Name(), "SelfHealing.VeteranMax", SelfHealingMax[2]);
	SelfHealingMax[3] = ini.Get_Float(Name(), "SelfHealing.EliteMax", SelfHealingMax[3]);
	SelfHealingCombatDelay = std::max(ini.Get_Int(Name(), "SelfHealing.CombatDelay", SelfHealingCombatDelay), 0);
	IsMechanic = ini.Get_Bool(Name(), "Mechanic", IsMechanic);
	IsOmniHealer = ini.Get_Bool(Name(), "OmniHealer", IsOmniHealer);
	IsDisableable = ini.Get_Bool(Name(), "Disableable", IsDisableable);
	IsTiberiumHeal = ini.Get_Bool(Name(), "TiberiumHeal", IsTiberiumHeal);
	IsImmuneToVeins = ini.Get_Bool(Name(), "ImmuneToVeins", IsImmuneToVeins);
	if (ini.Is_Present(Name(), "ImmuneToEMP")) {
		IsImmuneToEMP = ini.Get_Bool(Name(), "ImmuneToEMP", false);
	}

	VeteranAbilities = ini.Get_Abilities(Name(), "VeteranAbilities", VeteranAbilities);
	EliteAbilities = ini.Get_Abilities(Name(), "EliteAbilities", EliteAbilities);
}


void TechnoTypeClass::Read_Targeting_INI(CCINIClass const & ini)
{
	ThreatRange = ini.Get_Lepton(Name(), "GuardRange", ThreatRange);
	IsCanPassiveAquire = ini.Get_Bool(Name(), "CanPassiveAquire", IsCanPassiveAquire);
	IsCanRetaliate = ini.Get_Bool(Name(), "CanRetaliate", IsCanRetaliate);
	IsHoverAttack = ini.Get_Bool(Name(), "HoverAttack", IsHoverAttack);
	IsOpportunityFire = ini.Get_Bool(Name(), "OpportunityFire", IsOpportunityFire);
	IsAttackFriendlies = ini.Get_Bool(Name(), "AttackFriendlies", IsAttackFriendlies);
	IsAttackCursorOnFriendlies = ini.Get_Bool(Name(), "AttackCursorOnFriendlies", IsAttackCursorOnFriendlies);
	IsDefaultToGuardArea = ini.Get_Bool(Name(), "DefaultToGuardArea", IsDefaultToGuardArea);

	char value[64];
	if (ini.Get_String(Name(), "VHPScan", "", value, sizeof(value)) > 0) {
		VHPScan = VHPScan_Named(value, VHPScan);
	}

	IsCloseRange = ini.Get_Bool(Name(), "CloseRange", IsCloseRange);
	NavalTargeting = ini.Get_Int(Name(), "NavalTargeting", NavalTargeting);
	LandTargeting = ini.Get_Int(Name(), "LandTargeting", LandTargeting);
	IsDistributedFire = ini.Get_Bool(Name(), "DistributedFire", IsDistributedFire);
	IsCanApproachTarget = ini.Get_Bool(Name(), "CanApproachTarget", IsCanApproachTarget);
	IsCanRecalcApproachTarget = ini.Get_Bool(Name(), "CanRecalcApproachTarget", IsCanRecalcApproachTarget);
	IsStupidHunt = ini.Get_Bool(Name(), "StupidHunt", IsStupidHunt);
	IsNoAutoFire = ini.Get_Bool(Name(), "NoAutoFire", IsNoAutoFire);
	IsTargetLaser = ini.Get_Bool(Name(), "TargetLaser", IsTargetLaser);
}


void TechnoTypeClass::Read_Bounty_INI(CCINIClass const & ini)
{
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
}


void TechnoTypeClass::Read_Insignia_INI(CCINIClass const & ini)
{
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
}


void TechnoTypeClass::Read_Attach_Effect_INI(CCINIClass const & ini)
{
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
}


void TechnoTypeClass::Read_Spawns_INI(CCINIClass const & ini)
{
	IsSlaved = ini.Get_Bool(Name(), "Slaved", IsSlaved);
	Spawns = TGet_Class(ini, Name(), "Spawns", Spawns);
	SpawnsNumber = ini.Get_Int(Name(), "SpawnsNumber", SpawnsNumber);
	SpawnRegenRate = ini.Get_Int(Name(), "SpawnRegenRate", SpawnRegenRate);
	SpawnReloadRate = ini.Get_Int(Name(), "SpawnReloadRate", SpawnReloadRate);
	IsSpawned = ini.Get_Bool(Name(), "Spawned", IsSpawned);
	IsMissileSpawn = ini.Get_Bool(Name(), "MissileSpawn", IsMissileSpawn);
	Enslaves = TGet_Class(ini, Name(), "Enslaves", Enslaves);
	SlavesNumber = ini.Get_Int(Name(), "SlavesNumber", SlavesNumber);
	SlaveRegenRate = ini.Get_Int(Name(), "SlaveRegenRate", SlaveRegenRate);
	SlaveReloadRate = ini.Get_Int(Name(), "SlaveReloadRate", SlaveReloadRate);
	IsPoweredUnit = ini.Get_Bool(Name(), "PoweredUnit", IsPoweredUnit);
	PowersUnit = TGet_Class(ini, Name(), "PowersUnit", PowersUnit);
	PoweredBy = TGet_TypeList<BuildingTypeClass>(ini, Name(), "PoweredBy", PoweredBy);

	std::string const operators = ini.Get_String(Name(), "Operator");
	if (!operators.empty()) {
		Operators.Clear();
		IsOperatorAny = false;
		for (std::string const & name : Split_List(operators, ',', " \t", true)) {
			if (stricmp(name.c_str(), "_ANY_") == 0) {
				IsOperatorAny = true;
			} else if (TechnoTypeClass const * type = TechnoTypeClass::From_Name(name.c_str())) {
				Operators.Add(type);
			} else if (stricmp(name.c_str(), "none") != 0) {
				DebugString("%s: Operator names no such object type: %s; skipped.\n", Name(), name.c_str());
			}
		}
	}
}


void TechnoTypeClass::Read_Jumpjet_INI(CCINIClass const & ini)
{
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
	JumpjetCrash = ini.Get_Float(Name(), "JumpjetCrash", JumpjetCrash);
	JumpjetHeight = ini.Get_Int(Name(), "JumpjetHeight", JumpjetHeight);
	JumpjetAccel = ini.Get_Float(Name(), "JumpjetAccel", JumpjetAccel);
	JumpjetWobbles = ini.Get_Float(Name(), "JumpjetWobbles", JumpjetWobbles);
	JumpjetDeviation = ini.Get_Int(Name(), "JumpjetDeviation", JumpjetDeviation);
	IsJumpjetNoWobbles = ini.Get_Bool(Name(), "JumpjetNoWobbles", IsJumpjetNoWobbles);
}


void TechnoTypeClass::Read_Airstrike_INI(CCINIClass const & ini)
{
	AirstrikeTeam = ini.Get_Int(Name(), "AirstrikeTeam", AirstrikeTeam);
	EliteAirstrikeTeam = ini.Get_Int(Name(), "EliteAirstrikeTeam", EliteAirstrikeTeam);
	AirstrikeTeamType = TGet_Class(ini, Name(), "AirstrikeTeamType", AirstrikeTeamType);
	EliteAirstrikeTeamType = TGet_Class(ini, Name(), "EliteAirstrikeTeamType", EliteAirstrikeTeamType);
	AirstrikeRechargeTime = ini.Get_Int(Name(), "AirstrikeRechargeTime", AirstrikeRechargeTime);
	EliteAirstrikeRechargeTime = ini.Get_Int(Name(), "EliteAirstrikeRechargeTime", EliteAirstrikeRechargeTime);
}


void TechnoTypeClass::Read_Destruction_INI(CCINIClass const & ini)
{
	IsExploding = ini.Get_Bool(Name(), "Explodes", IsExploding);
	MaxDebris = ini.Get_Int(Name(), "MaxDebris", MaxDebris);
	MinDebris = ini.Get_Int(Name(), "MinDebris", MinDebris);
	if (MinDebris < 0) {
		MinDebris = 0;
	}
	if (MaxDebris < MinDebris) {
		MaxDebris = MinDebris;
	}

	IsCrashable = ini.Get_Bool(Name(), "Crashable", IsCrashable);
	DebrisTypes = TGet_TypeList<VoxelAnimTypeClass>(ini, IniName, "DebrisTypes", DebrisTypes);
	DebrisMaximums = ini.Get_IntList(IniName, "DebrisMaximums", DebrisMaximums);
	DebrisAnims = TGet_TypeList<AnimTypeClass>(ini, IniName, "DebrisAnims", DebrisAnims);
	DestroyAnim = TGet_TypeList<AnimTypeClass>(ini, IniName, "DestroyAnim", DestroyAnim);
}


void TechnoTypeClass::Read_Weapons_INI(CCINIClass const & ini)
{
	BombSight = ini.Get_Int(Name(), "BombSight", BombSight);
	TurretCount = ini.Get_Int(Name(), "TurretCount", TurretCount);
	WeaponCount = ini.Get_Int(Name(), "WeaponCount", WeaponCount);
	IsClearAllWeapons = ini.Get_Bool(Name(), "ClearAllWeapons", IsClearAllWeapons);
	IsMobileFire = ini.Get_Bool(Name(), "MobileFire", IsMobileFire);
	IsPreventAttackMove = ini.Get_Bool(Name(), "PreventAttackMove", IsPreventAttackMove);
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
	if (IsClearAllWeapons) {
		Weapons[0].Weapon = NULL;
		Weapons[1].Weapon = NULL;
		EliteWeapons[0].Weapon = NULL;
		EliteWeapons[1].Weapon = NULL;
	}

	AirRangeBonus = ini.Get_Lepton(Name(), "AirRangeBonus", AirRangeBonus);
	RadialFireSegments = ini.Get_Int(Name(), "RadialFireSegments", RadialFireSegments);
	FireAngle = ini.Get_Int(Name(), "FireAngle", FireAngle);
	IsLeader = false;
	if (Weapons[0].Weapon != NULL && Weapons[0].Weapon->Attack > 0) {
		IsLeader = true;
	}
}


void TechnoTypeClass::Read_Transport_INI(CCINIClass const & ini)
{
	IsGunner = ini.Get_Bool(Name(), "Gunner", IsGunner);
	IFVMode = ini.Get_Int(Name(), "IFVMode", IFVMode);

	if (ini.Is_Present(Name(), "OpenTopped.RangeBonus")) {
		OpenToppedRangeBonus = ini.Get_Int(Name(), "OpenTopped.RangeBonus", 0);
	}
	if (ini.Is_Present(Name(), "OpenTopped.DamageMultiplier")) {
		OpenToppedDamageMultiplier = ini.Get_Float(Name(), "OpenTopped.DamageMultiplier", 1.0);
	}
	if (ini.Is_Present(Name(), "OpenTopped.WarpDistance")) {
		OpenToppedWarpDistance = ini.Get_Int(Name(), "OpenTopped.WarpDistance", 0);
	}
	if (ini.Is_Present(Name(), "OpenTransport.RangeBonus")) {
		OpenTransportRangeBonus = ini.Get_Int(Name(), "OpenTransport.RangeBonus", 0);
	}
	if (ini.Is_Present(Name(), "OpenTransport.DamageMultiplier")) {
		OpenTransportDamageMultiplier = ini.Get_Float(Name(), "OpenTransport.DamageMultiplier", 1.0);
	}
	IsOpenTopped = ini.Get_Bool(Name(), "OpenTopped", IsOpenTopped);
	OpenTransportWeapon = ini.Get_Int(Name(), "OpenTransportWeapon", OpenTransportWeapon);
	MaxPassengers = ini.Get_Int(Name(), "Passengers", MaxPassengers);
	Size = ini.Get_Int(Name(), "Size", Size);
	SizeLimit = ini.Get_Int(Name(), "SizeLimit", SizeLimit);
	IsVehicleTransport = ini.Get_Bool(Name(), "IsVehicleTransport", IsVehicleTransport);
}


void TechnoTypeClass::Read_Gattling_INI(CCINIClass const & ini)
{
	IsGattling = ini.Get_Bool(Name(), "IsGattling", IsGattling);
	IsChargeTurret = ini.Get_Bool(Name(), "IsChargeTurret", IsChargeTurret);
	IsPassengerTurret = ini.Get_Bool(Name(), "PassengerTurret", IsPassengerTurret);
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
}


void TechnoTypeClass::Read_Deploy_INI(CCINIClass const & ini)
{
	IsDeployFire = ini.Get_Bool(Name(), "DeployFire", IsDeployFire);
	DeployFireWeapon = ini.Get_Int(Name(), "DeployFireWeapon", DeployFireWeapon);
	IsDeployToLand = ini.Get_Bool(Name(), "DeployToLand", IsDeployToLand);
	UndeployDelay = ini.Get_Int(Name(), "UndeployDelay", UndeployDelay);
	Dock = TGet_TypeList<BuildingTypeClass>(ini, Name(), "Dock", Dock);
	DeploysInto = TGet_Class(ini, Name(), "DeploysInto", DeploysInto);
	UndeploysInto = TGet_Class(ini, Name(), "UndeploysInto", UndeploysInto);
	UnloadingClass = TGet_Class(ini, Name(), "UnloadingClass", UnloadingClass);
	DeployTime = ini.Get_Float(Name(), "DeployTime", DeployTime);
}


void TechnoTypeClass::Read_Turret_INI(CCINIClass const & ini)
{
	IsRadarEquipped = ini.Get_Bool(Name(), "TurretSpins", IsRadarEquipped);
	IsTurretEquipped = ini.Get_Bool(Name(), "Turret", IsTurretEquipped);
	IsHasTurretTooltips = ini.Get_Bool(Name(), "HasTurretTooltips", IsHasTurretTooltips);
	// Accepted from Yuri's Revenge rules, but nothing uses it.
	ini.Get_Bool(Name(), "HasTurretTooltips", false);
	IsTurretRecoil = ini.Get_Bool(Name(), "TurretRecoil", IsTurretRecoil);
	TurretRecoilControl.Travel = ini.Get_Int(Name(), "TurretTravel", TurretRecoilControl.Travel);
	TurretRecoilControl.CompressFrames = ini.Get_Int(Name(), "TurretCompressFrames", TurretRecoilControl.CompressFrames);
	Keep_Recoil_Frames(TurretRecoilControl.CompressFrames);
	TurretRecoilControl.HoldFrames = ini.Get_Int(Name(), "TurretHoldFrames", TurretRecoilControl.HoldFrames);
	Keep_Recoil_Frames(TurretRecoilControl.HoldFrames);
	TurretRecoilControl.RecoverFrames = ini.Get_Int(Name(), "TurretRecoverFrames", TurretRecoilControl.RecoverFrames);
	Keep_Recoil_Frames(TurretRecoilControl.RecoverFrames);

	// gamemd copies the turret values into the barrel before it reads the barrel keys (TechnoTypeClass::LoadFromINI, 0x712170),
	// so a barrel key that is absent keeps the turret's value rather than the constructor default.
	BarrelRecoilControl = TurretRecoilControl;
	BarrelRecoilControl.Travel = ini.Get_Int(Name(), "BarrelTravel", BarrelRecoilControl.Travel);
	BarrelRecoilControl.CompressFrames = ini.Get_Int(Name(), "BarrelCompressFrames", BarrelRecoilControl.CompressFrames);
	Keep_Recoil_Frames(BarrelRecoilControl.CompressFrames);
	BarrelRecoilControl.HoldFrames = ini.Get_Int(Name(), "BarrelHoldFrames", BarrelRecoilControl.HoldFrames);
	Keep_Recoil_Frames(BarrelRecoilControl.HoldFrames);
	BarrelRecoilControl.RecoverFrames = ini.Get_Int(Name(), "BarrelRecoverFrames", BarrelRecoilControl.RecoverFrames);
	Keep_Recoil_Frames(BarrelRecoilControl.RecoverFrames);
}


void TechnoTypeClass::Read_Effects_INI(CCINIClass const & ini)
{
	Explosion = TGet_TypeList<AnimTypeClass>(ini, Name(), "Explosion", Explosion);
	ScrapExplosion = TGet_TypeList<AnimTypeClass>(ini, Name(), "ScrapExplosion", ScrapExplosion);
	NaturalParticleSystem = TGet_Class(ini, Name(), "NaturalParticleSystem", NaturalParticleSystem);
	RefinerySmokeParticleSystem = TGet_Class(ini, Name(), "RefinerySmokeParticleSystem", RefinerySmokeParticleSystem);
	RefinerySmokeOffset[0] = ini.Get_Offset(Name(), "RefinerySmokeOffsetOne", RefinerySmokeOffset[0]);
	RefinerySmokeOffset[1] = ini.Get_Offset(Name(), "RefinerySmokeOffsetTwo", RefinerySmokeOffset[1]);
	RefinerySmokeOffset[2] = ini.Get_Offset(Name(), "RefinerySmokeOffsetThree", RefinerySmokeOffset[2]);
	RefinerySmokeOffset[3] = ini.Get_Offset(Name(), "RefinerySmokeOffsetFour", RefinerySmokeOffset[3]);
	NaturalParticleLocation = ini.Get_Offset(Name(), "NaturalParticleLocation", NaturalParticleLocation);
	DamageParticleSystems = TGet_TypeList<ParticleSystemTypeClass>(ini, Name(), "DamageParticleSystems", DamageParticleSystems);
	DamageSmokeOffset = ini.Get_Offset(Name(), "DamageSmokeOffset", DamageSmokeOffset);
	DamSmkOffScrnRel = ini.Get_Bool(Name(), "DamSmkOffScrnRel", DamSmkOffScrnRel);
	DestroyParticleSystems = TGet_TypeList<ParticleSystemTypeClass>(ini, Name(), "DestroyParticleSystems", DestroyParticleSystems);
	DestroySmokeOffset = ini.Get_Offset(Name(), "DestroySmokeOffset", DestroySmokeOffset);
}


void TechnoTypeClass::Read_Ammo_INI(CCINIClass const & ini)
{
	IsPipsDrawForAll = ini.Get_Bool(Name(), "PipsDrawForAll", IsPipsDrawForAll);
	InitialAmmo = ini.Get_Int(Name(), "InitialAmmo", InitialAmmo);
	IsManualReload = ini.Get_Bool(Name(), "ManualReload", IsManualReload);
	PipScale = ini.Get_PipScaleType(Name(), "PipScale", PipScale);
	if (ini.Is_Present(Name(), "MaxPips")) {
		MaxPips = std::max(ini.Get_Int(Name(), "MaxPips", 0), 0);
	}

	MaxAmmo = ini.Get_Int(Name(), "Ammo", MaxAmmo);
	ReloadDelay = ini.Get_Int(Name(), "Reload", ReloadDelay);
	EmptyReloadDelay = ini.Get_Int(Name(), "EmptyReload", EmptyReloadDelay);
	ReloadIncrement = ini.Get_Int(Name(), "ReloadIncrement", ReloadIncrement);
	ReloadAmount = ini.Get_Int(Name(), "ReloadAmount", ReloadAmount);
	EmptyReloadAmountSetting = ini.Get_Int(Name(), "EmptyReloadAmount", EmptyReloadAmountSetting);
	PipWrap = ini.Get_Int(Name(), "PipWrap", PipWrap);
}


void TechnoTypeClass::Read_Production_INI(CCINIClass const & ini)
{
	char value[64];
	IsDoubleOwned = ini.Get_Bool(Name(), "DoubleOwned", IsDoubleOwned);
	BuildTimeMultipleFactory = ini.Get_Float(Name(), "BuildTime.MultipleFactory", BuildTimeMultipleFactory);

	// gamemd multiplies build time by this float32, truncated from the INI value (TechnoClass::TimeToBuild, 0x6F47A0, fmul dword [type+0x608]).
	BuildTimeMultiplier = Truncate_To_Single(ini.Get_Double(Name(), "BuildTimeMultiplier", BuildTimeMultiplier));
	if (ini.Get_String(Name(), "Survivor.Side0", "", value, sizeof(value)) > 0) {
		SurvivorSide[0] = strcmpi(value, "<none>") == 0 ? "" : value;
	}
	if (ini.Get_String(Name(), "Survivor.Side1", "", value, sizeof(value)) > 0) {
		SurvivorSide[1] = strcmpi(value, "<none>") == 0 ? "" : value;
	}
	if (ini.Get_String(Name(), "Survivor.Side2", "", value, sizeof(value)) > 0) {
		SurvivorSide[2] = strcmpi(value, "<none>") == 0 ? "" : value;
	}
	if (ini.Get_String(Name(), "Survivor.Side3", "", value, sizeof(value)) > 0) {
		SurvivorSide[3] = strcmpi(value, "<none>") == 0 ? "" : value;
	}
	CrewTechnicianChance = ini.Get_Int(Name(), "Crew.TechnicianChance", CrewTechnicianChance);
	CrewEngineerChance = ini.Get_Int(Name(), "Crew.EngineerChance", CrewEngineerChance);
	SurvivorPilots = ini.Get_Int(Name(), "Survivor.Pilots", SurvivorPilots);
	SurvivorPilotChance[0] = ini.Get_Int(Name(), "Survivor.RookiePilotChance", SurvivorPilotChance[0]);
	SurvivorPilotChance[1] = ini.Get_Int(Name(), "Survivor.VeteranPilotChance", SurvivorPilotChance[1]);
	SurvivorPilotChance[2] = ini.Get_Int(Name(), "Survivor.ElitePilotChance", SurvivorPilotChance[2]);
	SurvivorPassengerChance[0] = ini.Get_Int(Name(), "Survivor.RookiePassengerChance", SurvivorPassengerChance[0]);
	SurvivorPassengerChance[1] = ini.Get_Int(Name(), "Survivor.VeteranPassengerChance", SurvivorPassengerChance[1]);
	SurvivorPassengerChance[2] = ini.Get_Int(Name(), "Survivor.ElitePassengerChance", SurvivorPassengerChance[2]);
	ParachuteAnim = TGet_Class(ini, Name(), "Parachute.Anim", ParachuteAnim);
	if (ini.Is_Present(Name(), "SlamSound")) {
		SlamSound = ini.Get_VocType(Name(), "SlamSound", VOC_NONE);
		IsSlamSoundSet = true;
	}
	RadialIndicatorRadius = ini.Get_Float(Name(), "RadialIndicatorRadius", RadialIndicatorRadius);
	WaterImage = TGet_Class(ini, Name(), "WaterImage", WaterImage);
	IsCloneable = ini.Get_Bool(Name(), "Cloneable", IsCloneable);
	ClonedAt = TGet_TypeList<BuildingTypeClass>(ini, Name(), "ClonedAt", ClonedAt);
	if (ini.Is_Present(Name(), "FactoryOwners")) {
		FactoryOwners = ObjectKeyRules::Split_List(ini.Get_String(Name(), "FactoryOwners"));
	}
	if (ini.Is_Present(Name(), "FactoryOwners.Forbidden")) {
		FactoryOwnersForbidden = ObjectKeyRules::Split_List(ini.Get_String(Name(), "FactoryOwners.Forbidden"));
	}
	if (ini.Get_String(Name(), "Cursor.Move", "", value, sizeof(value)) > 0) {
		CursorMove = value;
	}
	if (ini.Get_String(Name(), "Cursor.NoMove", "", value, sizeof(value)) > 0) {
		CursorNoMove = value;
	}
	if (ini.Get_String(Name(), "Cursor.Deploy", "", value, sizeof(value)) > 0) {
		CursorDeploy = value;
	}
	if (ini.Get_String(Name(), "Cursor.NoDeploy", "", value, sizeof(value)) > 0) {
		CursorNoDeploy = value;
	}
	if (ini.Get_String(Name(), "Cursor.Enter", "", value, sizeof(value)) > 0) {
		CursorEnter = value;
	}
	if (ini.Get_String(Name(), "Cursor.NoEnter", "", value, sizeof(value)) > 0) {
		CursorNoEnter = value;
	}
	if (ini.Is_Present(Name(), "CloakSound")) {
		CloakSound = ini.Get_VocType(Name(), "CloakSound", VOC_NONE);
		IsCloakSoundSet = true;
	}
	if (ini.Is_Present(Name(), "DecloakSound")) {
		DecloakSound = ini.Get_VocType(Name(), "DecloakSound", VOC_NONE);
		IsDecloakSoundSet = true;
	}
	CloakStages = ini.Get_Int(Name(), "Cloakable.Stages", CloakStages);
	IsCanPassiveAquireGuard = ini.Get_Bool(Name(), "CanPassiveAquire.Guard", IsCanPassiveAquireGuard);
	IsCanPassiveAquireCloak = ini.Get_Bool(Name(), "CanPassiveAquire.Cloak", IsCanPassiveAquireCloak);
	IsNoManualFire = ini.Get_Bool(Name(), "NoManualFire", IsNoManualFire);
	IsGattlingCycle = ini.Get_Bool(Name(), "Gattling.Cycle", IsGattlingCycle);
	if (ini.Get_String(Name(), "EVA.VeteranPromoted", "", value, sizeof(value)) > 0) {
		EvaPromoted[0] = value;
	}
	if (ini.Get_String(Name(), "EVA.ElitePromoted", "", value, sizeof(value)) > 0) {
		EvaPromoted[1] = value;
	}
	if (ini.Get_String(Name(), "EVA.Lost", "", value, sizeof(value)) > 0) {
		EvaLost = value;
		IsEvaLostSet = true;
	}
	BuildLimit = ini.Get_Int(Name(), "BuildLimit", BuildLimit);
	CameoSortOrder = ini.Get_Int(Name(), "CameoSortOrder", CameoSortOrder);
	Prerequisite = ini.Get_BuildingType_List(ini, IniName, "Prerequisite", Prerequisite);
	Prerequisite = ini.Get_BuildingType_List(ini, IniName, "Prerequisite.List0", Prerequisite);
	PrerequisiteLists.resize(std::max(0, ini.Get_Int(Name(), "Prerequisite.Lists", (int)PrerequisiteLists.size())));
	for (std::size_t index = 0; index < PrerequisiteLists.size(); index++) {
		std::string const key = "Prerequisite.List" + std::to_string(index + 1);
		PrerequisiteLists[index] = ini.Get_BuildingType_List(ini, IniName, key.c_str(), PrerequisiteLists[index]);
	}
	PrerequisiteNegative = ini.Get_BuildingType_List(ini, IniName, "Prerequisite.Negative", PrerequisiteNegative);
	if (ini.Is_Present(Name(), "Prerequisite.RequiredTheaters")) {
		PrerequisiteTheaters = ObjectKeyRules::Split_List(ini.Get_String(Name(), "Prerequisite.RequiredTheaters"));
	}
	if (ini.Is_Present(Name(), "Prerequisite.StolenTechs")) {
		PrerequisiteStolenTechs.clear();
		for (std::string const & index : ObjectKeyRules::Split_List(ini.Get_String(Name(), "Prerequisite.StolenTechs"))) {
			int const value = atoi(index.c_str());
			if (value >= 0) {
				PrerequisiteStolenTechs.push_back(value);
			}
		}
	}
	PrerequisiteOverride = ini.Get_BuildingType_List(ini, IniName, "PrerequisiteOverride", PrerequisiteOverride);
	Level = ini.Get_Int(Name(), "TechLevel", Level);
	Cost = ini.Get_Int(Name(), "Cost", Cost);
	Soylent = ini.Get_Int(Name(), "Soylent", Soylent);
	Ownable = ini.Get_Owners(Name(), "Owner", Ownable);
	RequiredHouses = ini.Get_Owners(Name(), "RequiredHouses", RequiredHouses);
	ForbiddenHouses = ini.Get_Owners(Name(), "ForbiddenHouses", ForbiddenHouses);
	AIBasePlanningSide = ini.Get_Int(Name(), "AIBasePlanningSide", AIBasePlanningSide);
	IsRequiresStolenAlliedTech = ini.Get_Bool(Name(), "RequiresStolenAlliedTech", IsRequiresStolenAlliedTech);
	IsRequiresStolenSovietTech = ini.Get_Bool(Name(), "RequiresStolenSovietTech", IsRequiresStolenSovietTech);
	IsRequiresStolenThirdTech = ini.Get_Bool(Name(), "RequiresStolenThirdTech", IsRequiresStolenThirdTech);
	IsAllowedToStartInMultiplayer = ini.Get_Bool(Name(), "AllowedToStartInMultiplayer", IsAllowedToStartInMultiplayer);
}


void TechnoTypeClass::Read_AI_INI(CCINIClass const & ini)
{
	LeadershipRating = ini.Get_Int(Name(), "LeadershipRating", LeadershipRating);
	Category = ini.Get_CategoryType(Name(), "Category", Category);
	Risk = ini.Get_Int(Name(), "ThreatPosed", Risk);
	IsToProtect = ini.Get_Bool(Name(), "ToProtect", IsToProtect);
	MyEffectivenessCoefficient = ini.Get_Float(Name(), "MyEffectivenessCoefficient", MyEffectivenessCoefficient == 0 ? Rule->MyEffectivenessCoefficientDefault : MyEffectivenessCoefficient);
	TargetEffectivenessCoefficient = ini.Get_Float(Name(), "TargetEffectivenessCoefficient", TargetEffectivenessCoefficient == 0 ? Rule->TargetEffectivenessCoefficientDefault : TargetEffectivenessCoefficient);
	TargetSpecialThreatCoefficient = ini.Get_Float(Name(), "TargetSpecialThreatCoefficient", TargetSpecialThreatCoefficient == 0 ? Rule->TargetSpecialThreatCoefficientDefault : TargetSpecialThreatCoefficient);
	TargetStrengthCoefficient = ini.Get_Float(Name(), "TargetStrengthCoefficient", TargetStrengthCoefficient == 0 ? Rule->TargetStrengthCoefficientDefault : TargetStrengthCoefficient);
	TargetDistanceCoefficient = ini.Get_Float(Name(), "TargetDistanceCoefficient", TargetDistanceCoefficient == 0 ? Rule->TargetDistanceCoefficientDefault : TargetDistanceCoefficient);
	SpecialThreatValue = ini.Get_Float(Name(), "SpecialThreatValue", SpecialThreatValue);
}


void TechnoTypeClass::Read_Sounds_INI(CCINIClass const & ini)
{
	MindClearedSound = ini.Get_VocType(Name(), "MindClearedSound", MindClearedSound);
	PromoteVeteranSound = ini.Get_VocType(Name(), "Promote.VeteranSound", PromoteVeteranSound);
	PromoteEliteSound = ini.Get_VocType(Name(), "Promote.EliteSound", PromoteEliteSound);
	if (ini.Is_Present(Name(), "VoiceAirstrikeAttack")) {
		IsVoiceAirstrikeAttackSet = true;
		VoiceAirstrikeAttack = ini.Get_VocType(Name(), "VoiceAirstrikeAttack", VoiceAirstrikeAttack);
	}
	PromoteVeteranFlash = ini.Get_Int(Name(), "Promote.VeteranFlash", PromoteVeteranFlash);
	PromoteEliteFlash = ini.Get_Int(Name(), "Promote.EliteFlash", PromoteEliteFlash);
	ChronoInSound = ini.Get_VocType(Name(), "ChronoInSound", ChronoInSound);
	ChronoOutSound = ini.Get_VocType(Name(), "ChronoOutSound", ChronoOutSound);
	CreateSound = ini.Get_VocType(Name(), "CreateSound", CreateSound);
	EnterTransportSound = ini.Get_VocType(Name(), "EnterTransportSound", EnterTransportSound);
	LeaveTransportSound = ini.Get_VocType(Name(), "LeaveTransportSound", LeaveTransportSound);
	// Accepted from Yuri's Revenge rules, but nothing plays them: a grinder plays the rules' EnterGrinderSound.
	ini.Get_VocType(Name(), "EnterGrinderSound", VOC_NONE);
	ini.Get_VocType(Name(), "LeaveGrinderSound", VOC_NONE);
	LeaveBioReactorSound = ini.Get_VocType(Name(), "LeaveBioReactorSound", LeaveBioReactorSound);
	VoiceEnter = ini.Get_VocType(Name(), "VoiceEnter", VoiceEnter);
	VoiceDeploy = ini.Get_VocType(Name(), "VoiceDeploy", VoiceDeploy);
	VoiceUndeploy = ini.Get_VocType(Name(), "VoiceUndeploy", VoiceUndeploy);
	TurretRotateSound = ini.Get_VocType(Name(), "TurretRotateSound", TurretRotateSound);
	VoiceCapture = ini.Get_VocType(Name(), "VoiceCapture", VoiceCapture);
	VoiceHarvest = ini.Get_VocType(Name(), "VoiceHarvest", VoiceHarvest);
	ImpactLandSound = ini.Get_VocType(Name(), "ImpactLandSound", ImpactLandSound);
	ImpactWaterSound = ini.Get_VocType(Name(), "ImpactWaterSound", ImpactWaterSound);
	DamageSound = ini.Get_VocType(Name(), "DamageSound", DamageSound);
	VoicePrimaryWeaponAttack = ini.Get_VocType(Name(), "VoicePrimaryWeaponAttack", VoicePrimaryWeaponAttack);
	VoicePrimaryEliteWeaponAttack = ini.Get_VocType(Name(), "VoicePrimaryEliteWeaponAttack", VoicePrimaryEliteWeaponAttack);
	VoiceSecondaryWeaponAttack = ini.Get_VocType(Name(), "VoiceSecondaryWeaponAttack", VoiceSecondaryWeaponAttack);
	VoiceSecondaryEliteWeaponAttack = ini.Get_VocType(Name(), "VoiceSecondaryEliteWeaponAttack", VoiceSecondaryEliteWeaponAttack);
	ActivateSound = ini.Get_VocType(Name(), "ActivateSound", ActivateSound);
	DeactivateSound = ini.Get_VocType(Name(), "DeactivateSound", DeactivateSound);
	CrashingSound = ini.Get_VocType(Name(), "CrashingSound", CrashingSound);
	VoiceCrashing = ini.Get_VocType(Name(), "VoiceCrashing", VoiceCrashing);
	SinkingSound = ini.Get_VocType(Name(), "SinkingSound", SinkingSound);
	VoiceSinking = ini.Get_VocType(Name(), "VoiceSinking", VoiceSinking);
	VoiceMove = ini.Get_VocType_List(ini, IniName, "VoiceMove", VoiceMove);
	VoiceSpecialAttack = ini.Get_VocType_List(ini, IniName, "VoiceSpecialAttack", VoiceSpecialAttack);
	VoiceSelect = ini.Get_VocType_List(ini, IniName, "VoiceSelect", VoiceSelect);
	VoiceSelectEnslaved = ini.Get_VocType_List(ini, IniName, "VoiceSelectEnslaved", VoiceSelectEnslaved);
	VoiceSelectDeactivated = ini.Get_VocType_List(ini, IniName, "VoiceSelectDeactivated", VoiceSelectDeactivated);
	VoiceAttack = ini.Get_VocType_List(ini, IniName, "VoiceAttack", VoiceAttack);
	VoiceDie = ini.Get_VocType_List(ini, IniName, "VoiceDie", VoiceDie);
	DieSound = ini.Get_VocType_List(ini, IniName, "DieSound", DieSound);
	MoveSound = ini.Get_VocType_List(ini, IniName, "MoveSound", MoveSound);
	VoiceFeedback = ini.Get_VocType_List(ini, IniName, "VoiceFeedback", VoiceFeedback);
	AuxSound1 = ini.Get_VocType(Name(), "AuxSound1", AuxSound1);
	AuxSound2 = ini.Get_VocType(Name(), "AuxSound2", AuxSound2);
}


void TechnoTypeClass::Read_Art_INI(void)
{
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
	IsDisableVoxelCache = ArtINI.Get_Bool(Graphic_Name(), "DisableVoxelCache", IsDisableVoxelCache);
	IsDisableShadowCache = ArtINI.Get_Bool(Graphic_Name(), "DisableShadowCache", IsDisableShadowCache);
	if (ArtINI.Is_Present(Graphic_Name(), "Palette")) {
		PaletteName = ArtINI.Get_String(Graphic_Name(), "Palette");
	}

	char pcx[64];
	ArtINI.Get_String(Graphic_Name(), "CameoPCX", "", pcx, sizeof(pcx));
	CameoPCX = pcx;
	ArtINI.Get_String(Graphic_Name(), "AltCameoPCX", "", pcx, sizeof(pcx));
	AltCameoPCX = pcx;

	// gamemd TechnoTypeClass::LoadFromINI (0x712170) reads Palette= into PaletteFile, a 0x20-byte buffer.
	char palette[32];
	ArtINI.Get_String(Graphic_Name(), "Palette", "", palette, sizeof(palette));
	PaletteFile = palette;

	TStringID<24> cameo;
	if (ArtINI.Get_String(Graphic_Name(), "Cameo", "", cameo) > 0) {
		CameoFilename = cameo;
		_makepath(filename, NULL, NULL, CameoFilename, ".SHP");
		CameoData = (ShapeSet const *)MFCD::Retrieve(filename);
	}
	if (CameoData == NULL) {
		CameoData = (ShapeSet const *)MFCD::Retrieve("XXICON.SHP");
	}

	TStringID<24> altcameo;
	if (ArtINI.Get_String(Graphic_Name(), "AltCameo", "", altcameo) > 0) {
		_makepath(filename, NULL, NULL, altcameo, ".SHP");
		AltCameoData = (ShapeSet const *)MFCD::Retrieve(filename);
	}

	TurretNotExportedOnGround = ArtINI.Get_Bool(Graphic_Name(), "TurretNotExportedOnGround", TurretNotExportedOnGround);
	// gamemd reads the second launch point from the art section only (TechnoTypeClass::LoadFromINI, 0x712170).
	SecondSpawnOffset = ArtINI.Get_Point(Graphic_Name(), "SecondSpawnOffset", SecondSpawnOffset);
}


void TechnoTypeClass::Read_Weapon_FLH_INI(void)
{
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

	// Each passenger seat of an open-topped transport has its own mounting. A seat with no entry uses the primary mounting.
	for (int seat = 0; seat < 5; seat++) {
		char buf[32];
		sprintf(buf, "AlternateFLH%d", seat);
		AlternativeFLH[seat] = ArtINI.Get_Point(Graphic_Name(), buf, Weapons[0].FireFLH);
	}
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
			// TechnoTypeClass::GetPipMax (0x716290) answers the type's Ammo, so a row holds a pip for every shot.
			return(PipWrap != 0 ? MaxAmmo : std::min(MaxAmmo, MaxPips.value_or(MaxAmmo)));

		case PIPSCALE_TIBERIUM:
			return(MaxPips.value_or(Fetch_RTTI() == RTTI_INFANTRYTYPE ? 3 : 5));

		case PIPSCALE_PASSENGERS:
			return(std::min(MaxPassengers, MaxPips.value_or(MaxPassengers)));

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

		if (minrange && (coord - tcoord).Gamemd_Length() < minrange) {
			return(false);
		}

		if (weapon->Bullet->IsArcing) {
			int dist = (Point2D(coord) - Point2D(tcoord)).Gamemd_Length();
			if (dist > range) {
				return(false);
			}
			int height = tcoord.Z - coord.Z;
			double gravity = weapon->Bullet->IsFloater ? Get_Floater_Gravity() : Rule->Gravity;
			if (!Is_Projectile_Trajectory_Valid(weapon->MaxSpeed, dist, height, gravity) || (Map[tcoord].IsUnderBridge && tcoord.Z - coord.Z >= 3 * LEVEL_LEPTON_H)) {
				return(false);
			}
		} else {

			BuildingClass const * building = target->As_BuildingClass();
			if (building != NULL) {
				range += ((building->Class->Width() + building->Class->Height()) * (CELL_LEPTON_W / 4));
			}

			int cdist;
			if (RTTI != RTTI_AIRCRAFTTYPE) {
				cdist = (coord - tcoord).Gamemd_Length();
			} else {
				cdist = (Coord(Point2D(coord) - Point2D(tcoord), 0)).Gamemd_Length();
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
		}

		// gamemd tests no firing angle after the range (TechnoClass::InRange, 0x6F7220), so a Kirov directly over
		// its target, or a unit on a cliff firing straight down, still has the target in range.
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
		ArtINI.Get_String((const char *)GraphicName, "AltCameoPCX", "", buffer, sizeof(buffer));
		AltCameoPCX = buffer;

		AltCameoData = NULL;
		if (ArtINI.Get_String((const char *)GraphicName, "AltCameo", "", buffer, sizeof(buffer)) > 0) {
			_makepath(fname, NULL, NULL, buffer, ".SHP");
			AltCameoData = (const ShapeSet *)MFCD::Retrieve(fname);
		}

		Load_Insignia_Shapes();
}


/// <summary>
/// Tells whether an object of this type can take an attack-move order.
/// It needs a primary weapon, and PreventAttackMove=yes rules the order out.
/// </summary>
bool TechnoTypeClass::Can_Attack_Move(void) const
{
	return(Weapons[0].Weapon != NULL && !IsPreventAttackMove);
}


/// <summary>
/// The name an enemy sees on an object of this type: the text EnemyUIName names, or NULL when the type sets none.
/// </summary>
char const * TechnoTypeClass::Enemy_Full_Name(void) const
{
	return(Label_Text(EnemyUIName, EnemyUINameText));
}


std::string TechnoTypeClass::Select_Group(void) const
{
	std::string group = GroupAs.empty() ? std::string(Name()) : GroupAs;
	return(Upper(group));
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
	stream.Serialize(IdleRate);
	stream.Serialize(AirRate);
	stream.Serialize(RadarJamRadius);
	stream.Serialize(IsPipsDrawForAll);
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
	stream.Serialize(VoiceSelectEnslaved);
	stream.Serialize(VoiceSelectDeactivated);
	stream.Serialize(VoiceMove);
	stream.Serialize(VoiceSpecialAttack);
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
	stream.Serialize(IsHoverAttack);
	stream.Serialize(IsJumpJet);
	stream.Serialize(IsTiltCrashJumpjet);
	stream.Serialize(VHPScan);
	stream.Serialize(IsOpportunityFire);
	stream.Serialize(IsWarpable);
	stream.Serialize(IsImmuneToRadiation);
	stream.Serialize(IsImmuneToAbduction);
	stream.Serialize(IsProtectedDriver);
	stream.Serialize(ProtectedDriverMinHealth);
	stream.Serialize(IronCurtainModifier);
	stream.Serialize(ForceShieldModifier);
	stream.Serialize(IsGapGenerator);
	stream.Serialize(GapRadiusInCells);
	stream.Serialize(SuperGapRadiusInCells);
	stream.Serialize(IsDeployFire);
	stream.Serialize(DeployFireWeapon);
	stream.Serialize(IsDeployToLand);
	stream.Serialize(IsDisguiseWhenStill);
	stream.Serialize(IsSlaved);
	stream.Serialize(UndeployDelay);
	stream.Serialize(BombSight);
	stream.Serialize(DeathWeapon);
	stream.Serialize(DeathWeaponDamageModifier);
	stream.Serialize(CrushDamage[0]);
	stream.Serialize(CrushDamage[1]);
	stream.Serialize(CrushDamage[2]);
	stream.Serialize(CrushDamageWarhead);
	stream.Serialize(InitialAmmo);
	stream.Serialize(IsParasiteable);
	stream.Serialize(SuppressionThreshold);
	stream.Serialize(IsReselectIfLimboed);
	stream.Serialize(IsRejoinTeamIfLimboed);
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
	stream.Serialize(IsCanBeDriven);
	stream.Serialize(BuildTimeMultipleFactory);
	stream.Serialize(IsCrashable);
	stream.Serialize(IsConsideredAircraft);
	stream.Serialize(PromoteVeteranSound);
	stream.Serialize(PromoteEliteSound);
	stream.Serialize(IsVoiceAirstrikeAttackSet);
	stream.Serialize(VoiceAirstrikeAttack);
	stream.Serialize(PromoteVeteranFlash);
	stream.Serialize(PromoteEliteFlash);
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
	for (std::string & side : SurvivorSide) {
		stream.Serialize(side);
	}
	stream.Serialize(CrewTechnicianChance);
	stream.Serialize(CrewEngineerChance);
	stream.Serialize(SurvivorPilots);
	stream.Serialize(SurvivorPilotChance);
	stream.Serialize(SurvivorPassengerChance);
	stream.Serialize(PrerequisiteStolenTechs);
	stream.Serialize(NoAmmoWeapon);
	stream.Serialize(NoAmmoAmount);
	stream.Serialize(InitialPayloadTypes);
	stream.Serialize(InitialPayloadNums);
	stream.Serialize(IsNoManualEnter);
	stream.Serialize(IsNoManualUnload);
	stream.Serialize(PassengersAllowed);
	stream.Serialize(PassengersDisallowed);
	stream.Serialize(IsPassengersBySize);
	stream.Serialize(FallRateParachute);
	stream.Serialize(FallRateNoParachute);
	stream.Serialize(FallRateParachuteMax);
	stream.Serialize(FallRateNoParachuteMax);
	stream.Serialize(IsCivilianEnemy);
	stream.Serialize(IsOmniCrusherAggressive);
	stream.Serialize(IsExperienceFromPassengers);
	stream.Serialize(IsExperiencePromotePassengers);
	stream.Serialize(ExperiencePassengerModifier);
	stream.Serialize(IsExperienceFromAirstrike);
	stream.Serialize(ExperienceAirstrikeModifier);
	stream.Serialize(ExperienceMindControlSelfModifier);
	stream.Serialize(ExperienceMindControlVictimModifier);
	stream.Serialize(ExperienceSpawnOwnerModifier);
	stream.Serialize(ExperienceSpawnModifier);
	stream.Serialize(EnemyUIName);
	stream.Serialize(FakeOf);
	stream.Serialize(IsLowSelectionPriority);
	stream.Serialize(PaletteName);
	stream.Serialize(ParachuteAnim);
	stream.Serialize(SlamSound);
	stream.Serialize(IsSlamSoundSet);
	stream.Serialize(RadialIndicatorRadius);
	stream.Serialize(WaterImage);
	stream.Serialize(IsCloneable);
	stream.Serialize(ClonedAt);
	stream.Serialize(FactoryOwners);
	stream.Serialize(FactoryOwnersForbidden);
	stream.Serialize(CursorMove);
	stream.Serialize(CursorNoMove);
	stream.Serialize(CursorDeploy);
	stream.Serialize(CursorNoDeploy);
	stream.Serialize(CursorEnter);
	stream.Serialize(CursorNoEnter);
	stream.Serialize(CloakSound);
	stream.Serialize(DecloakSound);
	stream.Serialize(IsCloakSoundSet);
	stream.Serialize(IsDecloakSoundSet);
	stream.Serialize(CloakStages);
	stream.Serialize(IsCanPassiveAquireGuard);
	stream.Serialize(IsCanPassiveAquireCloak);
	stream.Serialize(IsNoManualFire);
	stream.Serialize(IsGattlingCycle);
	stream.Serialize(EvaPromoted[0]);
	stream.Serialize(EvaPromoted[1]);
	stream.Serialize(EvaLost);
	stream.Serialize(IsEvaLostSet);
	stream.Serialize(ChronoInSound);
	stream.Serialize(ChronoOutSound);
	stream.Serialize(CreateSound);
	stream.Serialize(EnterTransportSound);
	stream.Serialize(LeaveTransportSound);
	stream.Serialize(LeaveBioReactorSound);
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
	stream.Serialize(IsCloseRange);
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
	stream.Serialize(IsDrainLocal);
	stream.Serialize(DrainAmount);
	stream.Serialize(Enslaves);
	stream.Serialize(SlavesNumber);
	stream.Serialize(SlaveRegenRate);
	stream.Serialize(SlaveReloadRate);
	stream.Serialize(IsBunkerable);
	stream.Serialize(IsDistributedFire);
	stream.Serialize(IsCanApproachTarget);
	stream.Serialize(IsCanRecalcApproachTarget);
	stream.Serialize(IsStupidHunt);
	stream.Serialize(SensorsSight);
	stream.Serialize(IsPoweredUnit);
	stream.Serialize(ActivateSound);
	stream.Serialize(DeactivateSound);
	stream.Serialize(PowersUnit);
	stream.Serialize(PoweredBy);
	stream.Serialize(Operators);
	stream.Serialize(IsOperatorAny);
	stream.Serialize(CrashingSound);
	stream.Serialize(VoiceCrashing);
	stream.Serialize(SinkingSound);
	stream.Serialize(VoiceSinking);
	stream.Serialize(IsJumpjetDataSet);
	stream.Serialize(JumpjetTurnRate);
	stream.Serialize(JumpjetSpeed);
	stream.Serialize(JumpjetClimb);
	stream.Serialize(JumpjetCrash);
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
	stream.Serialize(IsMobileFire);
	stream.Serialize(IsPreventAttackMove);
	stream.Serialize(IsGattling);
	stream.Serialize(IsChargeTurret);
	stream.Serialize(IsPassengerTurret);
	stream.Serialize(WeaponStages);
	stream.Serialize(WeaponStage);
	stream.Serialize(EliteStage);
	stream.Serialize(RateUp);
	stream.Serialize(RateDown);
	stream.Serialize(IsGunner);
	stream.Serialize(IFVMode);
	stream.Serialize(AirRangeBonus);
	stream.Serialize(RadialFireSegments);
	stream.Serialize(OpenToppedRangeBonus);
	stream.Serialize(OpenToppedDamageMultiplier);
	stream.Serialize(OpenToppedWarpDistance);
	stream.Serialize(OpenTransportRangeBonus);
	stream.Serialize(OpenTransportDamageMultiplier);
	stream.Serialize(TurretWeapon);
	stream.Serialize(IsOpenTopped);
	stream.Serialize(OpenTransportWeapon);
	stream.Serialize(MaxPassengers);
	stream.Serialize(Size);
	stream.Serialize(SizeLimit);
	stream.Serialize(IsVehicleTransport);
	stream.Serialize(SightRange);
	stream.Serialize(DesignatorRange);
	stream.Serialize(InhibitorRange);
	stream.Serialize(Cost);
	stream.Serialize(Soylent);
	stream.Serialize(FlightLevel);
	stream.Serialize(Level);
	stream.Serialize(Prerequisite);
	stream.Serialize(PrerequisiteLists);
	stream.Serialize(PrerequisiteNegative);
	stream.Serialize(PrerequisiteTheaters);
	stream.Serialize(PrerequisiteOverride);
	stream.Serialize(Risk);
	stream.Serialize(Reward);
	stream.Serialize(MaxSpeed);
	stream.Serialize(Speed);
	stream.Serialize(MaxAmmo);
	stream.Serialize(ReloadDelay);
	stream.Serialize(EmptyReloadDelay);
	stream.Serialize(ReloadIncrement);
	stream.Serialize(ReloadAmount);
	stream.Serialize(EmptyReloadAmountSetting);
	stream.Serialize(PipWrap);
	stream.Serialize(Ownable);
	stream.Serialize(IsAllowedToStartInMultiplayer);
	stream.Serialize(CameoFilename);
	// CameoData -- artwork, fetched from the mix files again as this loads.
	stream.Serialize(CameoSortOrder);
	stream.Serialize(Rotation);
	stream.Serialize(ROT);
	stream.Serialize(TurretROT);
	stream.Serialize(TurretOffset);
	stream.Serialize(Points);
	stream.Serialize(Explosion);
	stream.Serialize(ScrapExplosion);
	stream.Serialize(NaturalParticleSystem);
	stream.Serialize(RefinerySmokeParticleSystem);
	for (int index = 0; index < 4; index++) {
		stream.Serialize(RefinerySmokeOffset[index]);
	}
	stream.Serialize(NaturalParticleLocation);
	stream.Serialize(DamageParticleSystems);
	stream.Serialize(DamageSmokeOffset);
	stream.Serialize(DamSmkOffScrnRel);
	stream.Serialize(DestroyParticleSystems);
	stream.Serialize(DestroySmokeOffset);
	stream.Serialize(ShadowIndex);
	stream.Serialize(Capacity);
	stream.Serialize(TurretNotExportedOnGround);
	stream.Serialize(Weapons);
	stream.Serialize(EliteWeapons);
	stream.Serialize(AlternativeFLH);
	stream.Serialize(IsTypeImmune);
	stream.Serialize(IsDamageReducesReadiness);
	stream.Serialize(IsSprayAttack);
	stream.Serialize(ReadinessReductionMultiplier);
	stream.Serialize(IsCanBeHidden);
	stream.Serialize(IsDetectDisguise);
	stream.Serialize(DetectDisguiseRange);
	stream.Serialize(IsMoveToShroud);
	stream.Serialize(IsTrainable);
	stream.Serialize(IsDontScore);
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
	stream.Serialize(IsHasTurretTooltips);
	stream.Serialize(IsTurretRecoil);
	stream.Serialize(TurretRecoilControl.Travel);
	stream.Serialize(TurretRecoilControl.CompressFrames);
	stream.Serialize(TurretRecoilControl.HoldFrames);
	stream.Serialize(TurretRecoilControl.RecoverFrames);
	stream.Serialize(BarrelRecoilControl.Travel);
	stream.Serialize(BarrelRecoilControl.CompressFrames);
	stream.Serialize(BarrelRecoilControl.HoldFrames);
	stream.Serialize(BarrelRecoilControl.RecoverFrames);
	stream.Serialize(IsRepairable);
	stream.Serialize(IsCrew);
	stream.Serialize(IsRemappable);
	stream.Serialize(IsCloakable);
	stream.Serialize(IsCloakAllowed);
	stream.Serialize(IsNoShadow);
	stream.Serialize(IsDisableVoxelCache);
	stream.Serialize(IsDisableShadowCache);
	stream.Serialize(IsSelfHealing);
	stream.Serialize(SelfHealingStep);
	stream.Serialize(SelfHealingRate);
	stream.Serialize(SelfHealingCap);
	for (int slot = 0; slot < SELF_HEAL_SLOTS; slot++) {
		stream.Serialize(SelfHealingAmount[slot]);
		stream.Serialize(SelfHealingMax[slot]);
	}
	stream.Serialize(SelfHealingCombatDelay);
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
	stream.Serialize(IsHunterSeekerIgnored);
	stream.Serialize(IsCrusher);
	stream.Serialize(IsPushy);
	stream.Serialize(IsOmniCrusher);
	stream.Serialize(IsOmniCrushResistant);
	stream.Serialize(IsTiltsWhenCrushes);
	stream.Serialize(IsSubterranean);
	stream.Serialize(IsAutoCrush);
	stream.Serialize(IsAccelerates);
	stream.Serialize(ZFudgeCliff);
	stream.Serialize(ZFudgeColumn);
	stream.Serialize(ZFudgeTunnel);
	stream.Serialize(ZFudgeBridge);
}

bool TechnoTypeClass::Has_Abductor_Weapon(void) const
{
	for (int slot = 0; slot < WEAPON_SLOT_COUNT; slot++) {
		if ((Weapons[slot].Weapon != NULL && Weapons[slot].Weapon->IsAbductor) || (EliteWeapons[slot].Weapon != NULL && EliteWeapons[slot].Weapon->IsAbductor)) {
			return(true);
		}
	}
	return(false);
}



/// <summary>
/// Adds this object type's rule data to a running checksum.
/// This routine is used by the multiplayer sync check to prove that every machine in the
/// game is playing by the same rules.
/// </summary>
void TechnoTypeClass::Compute_CRC(class CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(IsDontScore);
	crc(IsCanBeDriven);
	crc(RadarJamRadius);
	crc(NoAmmoWeapon);
	crc(NoAmmoAmount);
	crc((int)InitialPayloadTypes.size());
	crc((int)PassengersAllowed.size());
	crc((int)PassengersDisallowed.size());
	crc(FallRateParachute);
	crc(FallRateNoParachute);
	crc(FallRateParachuteMax);
	crc(FallRateNoParachuteMax);
	crc(ExperienceSpawnOwnerModifier);
	crc(ExperienceMindControlSelfModifier);
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
	crc(IsPushy);
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
	crc(VoiceSelectEnslaved.Count());
	crc(VoiceSelectDeactivated.Count());
	crc(VoiceMove.Count());
	crc(VoiceSpecialAttack.Count());
	crc(VoiceAttack.Count());
	crc(VoiceDie.Count());
	crc(VoiceFeedback.Count());
	crc(DamageParticleSystems.Count());
	crc(DestroyParticleSystems.Count());
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
	crc(PrerequisiteOverride.Count());
	crc(Risk);
	crc(Reward);
	crc(MaxSpeed);
	crc(Speed);
	crc(MaxAmmo);
	crc(ReloadDelay);
	crc(EmptyReloadDelay);
	crc(IsDamageReducesReadiness);
	crc(IsSprayAttack);
	crc(ReadinessReductionMultiplier);
	crc(ReloadIncrement);
	crc(ReloadAmount);
	crc(EmptyReloadAmountSetting);
	crc(PipWrap);
	crc((int)Ownable);
	crc(Rotation);
	crc(ROT);
	crc(TurretROT);
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
	crc(IsHasTurretTooltips);
	crc(IsTurretRecoil);
	crc(TurretRecoilControl.Travel);
	crc(TurretRecoilControl.CompressFrames);
	crc(TurretRecoilControl.HoldFrames);
	crc(TurretRecoilControl.RecoverFrames);
	crc(BarrelRecoilControl.Travel);
	crc(BarrelRecoilControl.CompressFrames);
	crc(BarrelRecoilControl.HoldFrames);
	crc(BarrelRecoilControl.RecoverFrames);
	crc(IsRepairable);
	crc(IsCrew);
	crc(IsRemappable);
	crc(IsCloakable);
	crc(IsCloakAllowed);
	crc(IsGapGenerator);
	crc(GapRadiusInCells);
	crc(IsNoShadow);
	crc(IsDisableVoxelCache);
	crc(IsDisableShadowCache);
	crc(IsSelfHealing);
	crc(SelfHealingStep);
	crc(SelfHealingRate);
	crc(SelfHealingCap);
	for (int slot = 0; slot < SELF_HEAL_SLOTS; slot++) {
		crc(SelfHealingAmount[slot]);
		crc(SelfHealingMax[slot]);
	}
	crc(SelfHealingCombatDelay);
	crc(IsMechanic);
	crc(IsOmniHealer);
	crc(IsExploding);
	crc(IsNoAutoFire);
	crc(IsRadarEquipped);
	crc(IsRegulated);
	crc(IsManualReload);
	crc(IsVisibleLoad);
	crc(IsLightningRod);
	crc(IsJumpJet);
	crc(IsTiltCrashJumpjet);
	crc(VHPScan);
	crc(SuperGapRadiusInCells);
	crc(IsCloseRange);
	crc(IsCanRecalcApproachTarget);
	crc(IsStupidHunt);
	crc(IsConsideredAircraft);
	crc(IsRejoinTeamIfLimboed);
	crc(JumpjetCrash);
	crc(IsMobileFire);
	crc(IsPreventAttackMove);
	crc(IsChargeTurret);
	crc(IsPassengerTurret);
	crc(RadialFireSegments);
	crc(IsOmniCrusher);
	crc(IsOmniCrushResistant);
	crc(IsHunterSeekerIgnored);
	for (size_t seat = 0; seat < AlternativeFLH.size(); seat++) {
		crc(AlternativeFLH[seat].X);
		crc(AlternativeFLH[seat].Y);
		crc(AlternativeFLH[seat].Z);
	}
}


/// <summary>
/// Fetches the offset that this type's damage smoke and spark particle systems are placed at, from the object's anchor.
/// Without DamSmkOffScrnRel the offset is DamageSmokeOffset as written. With it, X and Y are projected through the
/// tactical view's coordinate-to-pixel matrix and truncated to whole pixels, then used as lepton values, and Z lifts the
/// particle on the screen. With no tactical map to project through, the offset is returned as written.
/// </summary>
/// <returns>Returns with the offset in leptons.</returns>
Coord TechnoTypeClass::Particle_System_Offset(void) const
{
	if (!DamSmkOffScrnRel || TacticalMap == NULL) {
		return(Coord(DamageSmokeOffset.X, DamageSmokeOffset.Y, DamageSmokeOffset.Z));
	}

	Vector3 const pixel = TacticalMap->CoordToPixelMatrix * Vector3((float)DamageSmokeOffset.X, (float)DamageSmokeOffset.Y, 0.0f);
	int const z = DamageSmokeOffset.Z;
	int const lift = (int)std::sqrt(2.0 * (10.0 * z) * (10.0 * z));
	return(Coord((int)pixel.X - 10 * z, (int)pixel.Y - 10 * z, z >= 0 ? -lift : lift));
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


/// <summary>
/// Finds the object type whose INI name matches, ignoring case.
/// </summary>
/// <returns>TechnoTypeClass *; The first techno type with that name, or NULL when none has it.</returns>
TechnoTypeClass * TechnoTypeClass::From_Name(char const * name)
{
	for (int index = 0; index < TechnoTypes.Count(); index++) {
		if (stricmp(TechnoTypes[index]->Name(), name) == 0) {
			return(TechnoTypes[index]);
		}
	}
	return(NULL);
}
