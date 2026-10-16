# Superweapon extensions (Ares and Phobos)

Sources: the Ares documentation pages under `new/superweapons/` (December 2020 build, plain-text copies; Ares is closed source, so every Ares statement here comes from that documentation and is unverified), and Phobos `docs/New-or-Enhanced-Logics.md` section "Super Weapons" with `src/Ext/SWType/Body.cpp`, `Body.h`, `FireSuperWeapon.cpp`, `SWHelpers.cpp`, `Hooks.cpp`, `src/Ext/House/Hooks.cpp` (AI delay), `src/Ext/Building/Hooks.cpp` (EMPulse) and `src/New/Type/Affiliated/TypeConvertGroup.cpp`.

Scope: the Ares `SW.*` framework and its per-`Type=` keys, plus the ten Phobos additions listed under "Super Weapons". `Convert.*` on a superweapon is specified in `phobos-autodeath-convert-animunit.md`, and `SpyEffect.*` superweapon keys in `ui-and-spy-small-flags.md`; this file only notes where they meet. Phobos keys outside that list (`ShowTimer.*`, `SuperWeaponSidebar.*`, `UIDescription`, `CameoPriority`, `UseWeeds*`, `ShowDesignatorRange`, the warhead `LaunchSW*` keys) are not covered. Phobos needs Ares for most of this (it patches Ares functions and calls Ares type conversion), so a mod written for both sees the Ares behaviour with the Phobos additions on top. OpenYR has neither, so it must supply both layers.

## Keys

All keys are in a `[SuperWeaponType]` section of `rules(md).ini` unless a table says otherwise. The Source column is `A` for Ares (documentation only) and `P` for Phobos (read from its code).

### Keys the engine already reads

`SuperWeaponTypeClass::Read_INI` (`code/suprtype.cpp`) reads these. Everything outside this table is new.

| Key | Type | Notes |
|---|---|---|
| `Type` | behaviour name | Only the 18 names in `Special_From_Name` are known; an unknown name leaves the previous value. |
| `Action` | action name | Chooses the targeting cursor and the click. |
| `WeaponType` | WeaponType | Used by missile launches. |
| `RechargeTime` | float, minutes | `0` counts as unset. |
| `RechargeVoice`, `ChargingVoice`, `ImpatientVoice`, `SuspendVoice` | EVA speech | `RechargeVoice` is replaced by the per-behaviour EVA line when an EVA file is loaded. |
| `StartSound`, `SpecialSound` | sound | Used by the force shield. |
| `IsPowered`, `ManualControl`, `DisableableFromShell`, `ShowTimer`, `UseChargeDrain`, `AIDefendAgainst` | boolean | |
| `AuxBuilding` | BuildingType | Checked for structure grants only; plugs skip it. |
| `PreClick`, `PostClick`, `PreDependent` | boolean, boolean, behaviour name | Chronosphere pairing. Ares hardcodes these and ignores the INI (decision 21). |
| `SidebarImage`, `SidebarPCX`, `CameoSortOrder` | string, string, integer | `SidebarPCX` and `CameoSortOrder` are not on the Ares superweapon pages. |
| `SuperWeapon`, `SuperWeapon2` (BuildingType) | SuperWeaponType | `BuildingTypeClass::Read_INI` in `code/builtype.cpp`. |

### Identity and availability

| Key | Src | Type | Default |
|---|---|---|---|
| `SW.RequiredHouses` | A | list of countries | all |
| `SW.ForbiddenHouses` | A | list of countries | none |
| `SW.AuxBuildings` | A | list of BuildingTypes | none |
| `SW.NegBuildings` | A | list of BuildingTypes | none |
| `SW.AuxTechnos` | P | list of TechnoTypes | none |
| `SW.NegTechnos` | P | list of TechnoTypes | none |
| `SW.TechLevel` | P | integer | `0` |
| `SW.AllowPlayer`, `SW.AllowAI` | A | boolean | `yes` |
| `SW.Shots` | A | integer, `-1` unlimited | `-1` |
| `SW.AlwaysGranted` | A | boolean | `no` |
| `SW.Group` | A | integer | `0` |
| `SW.MaxCount` | A | integer, negative unlimited | `1` for EMPulse and HunterSeeker; other types unstated |
| `SuperWeapons` (BuildingType) | A | list of SuperWeaponTypes | none |

### Charge and cost

| Key | Src | Type | Default |
|---|---|---|---|
| `SW.InitialReady` | A | boolean | `no` |
| `SW.VirtualCharge` | A | boolean | `no` |
| `SW.ChargeToDrainRatio` | A | float, not 0 | `[General] ChargeToDrainRatio` |
| `SW.Unstoppable` | A | boolean | `no` |
| `Money.Amount` | A | integer credits, negative takes | `0` |
| `Money.DrainAmount` | A | integer credits | `0` |
| `Money.DrainDelay` | A | integer frames | `0` |
| `SW.Deferment` | A | integer frames | by Type (see Behaviour) |

### Targeting and range

| Key | Src | Type | Default |
|---|---|---|---|
| `SW.Range` | A | float radius in cells, or two integers width,height | by Type |
| `SW.AffectsHouse` | A | list: none, owner, allies, team, enemies, all | by Type |
| `SW.AffectsTarget` | A | list: none, land, water, empty, infantry, units, buildings | by Type |
| `SW.RangeMinimum`, `SW.RangeMaximum` | A | float cells, negative means Type default | `-1.0` |
| `SW.Designators` | A | list of TechnoTypes | none |
| `SW.AnyDesignator` | A | boolean | `no` |
| `SW.Inhibitors` | A | list of TechnoTypes | none |
| `SW.AnyInhibitor` | A | boolean | `no` |
| `SW.FireIntoShroud` | A | boolean | `yes` |
| `SW.RequiresTarget` | A | same list as `SW.AffectsTarget` | by Type |
| `SW.RequiresHouse` | A | list: none, owner, allies, team, enemies, all | by Type (`none` unless stated) |
| `SW.UseAITargeting`, `SW.AutoFire` | A | boolean | `no` |
| `SW.ManualFire` | A | boolean | `yes` |
| `SW.AITargeting` | A | mode (21 names, see Behaviour) | by Type |
| `SW.AIRequiresTarget` | A | list: none, land, water, empty, infantry, units, buildings, all | by mode |
| `SW.AIRequiresHouse` | A | list: owner, allies, team, enemies, others, all | by mode |
| `SW.AITargeting.Constraints` | A | list of 10 names | by Type |
| `SW.AITargeting.Preference` | A | None, Offensive or Defensive | by Type |
| `AISuperWeaponDelay` (`[General]`) | P | integer frames | unset (stock timing) |

### Effect, warhead and chaining

| Key | Src | Type | Default |
|---|---|---|---|
| `SW.Warhead` | A | WarheadType | by Type |
| `SW.Damage` | A | integer | by Type |
| `SW.PostDependent` | A | SuperWeaponType | first ChronoWarp in the list (ChronoSphere only) |
| `Detonate.Warhead` | P | WarheadType | none |
| `Detonate.Warhead.Full` | P | boolean | `yes` |
| `Detonate.Weapon` | P | WeaponType | none |
| `Detonate.Damage` | P | integer | weapon's `Damage` for a weapon, `0` for a warhead |
| `Detonate.AtFirer` | P | boolean | `no` |
| `SW.Next` | P | list of SuperWeaponTypes | none |
| `SW.Next.RealLaunch` | P | boolean | `yes` |
| `SW.Next.IgnoreInhibitors` | P | boolean | `no` |
| `SW.Next.IgnoreDesignators` | P | boolean | `yes` |
| `SW.Next.RollChances` | P | list of percentages | one sure roll |
| `SW.Next.RandomWeightsN` (N = 0, 1, ...; bare `SW.Next.RandomWeights` is N = 0) | P | list of integers | none |
| `SW.Link` | P | list of SuperWeaponTypes | none |
| `SW.Link.Grant`, `SW.Link.Reset` | P | boolean | `no` |
| `SW.Link.Ready` | P | boolean | `no` (a granted weapon uses its own `SW.InitialReady`) |
| `SW.Link.RollChances`, `SW.Link.RandomWeightsN` | P | as for `SW.Next` | as for `SW.Next` |
| `LimboDelivery.Types` | P | list of BuildingTypes | none |
| `LimboDelivery.IDs` | P | list of integers (not `-1`) | none |
| `LimboDelivery.RollChances`, `LimboDelivery.RandomWeightsN` | P | as for `SW.Next` | as for `SW.Next` |
| `LimboKill.AffectsHouse` (old spelling `LimboKill.Affected` still read, with a log warning) | P | house enumeration (none, owner/self, allies, team, enemies, neutral, all) | `owner` |
| `LimboKill.IDs` | P | list of integers | none |
| `LimboKill.Counts` | P | list of integers | no limit |
| `ConvertN.From`, `ConvertN.To`, `ConvertN.AffectsHouse`, and the bare `Convert.*` form | P | see `phobos-autodeath-convert-animunit.md` | `owner` |

### Presentation

| Key | Src | Type | Default |
|---|---|---|---|
| `Cursor` | A | mouse cursor name | `Attack`, or a Type default |
| `NoCursor` | A | mouse cursor name | `Disallowed`, or a Type default |
| `EVA.Detected`, `EVA.Ready`, `EVA.Activated` | A | EVA event, `none` disables | stock line; DropPod and HunterSeeker have their own |
| `EVA.Impatient`, `EVA.SelectTarget` | A | EVA event | stock line |
| `EVA.InsufficientFunds` | A | EVA event | `EVA_InsufficientFunds` |
| `Message.Detected`, `Message.Ready`, `Message.Launch`, `Message.Activate`, `Message.Abort`, `Message.InsufficientFunds` | A | CSF label | none |
| `Message.CannotFire` | A | CSF label | `MSG:CannotFire` |
| `Message.FirerColor` | A | boolean | `no` |
| `Message.Color` | A | color scheme | player's scheme |
| `Message.Activated.Owner`, `.Allies`, `.Enemies` | P | CSF label | none |
| `EVA.Activated.Owner`, `.Allies`, `.Enemies` | P | EVA event | none |
| `Message.LinkedSWAcquired` | P | CSF label | none |
| `EVA.LinkedSWAcquired` | P | EVA event | none |
| `Text.Hold`, `Text.Ready`, `Text.Charging`, `Text.Active`, `Text.Preparing` | A | CSF label | stock captions; HunterSeeker `Text.Ready` is `TXT_RELEASE` |
| `Light.Enabled` | A | boolean | not stated |
| `Light.Ambient`, `Light.Red`, `Light.Green`, `Light.Blue` | A | integer, `-1` for the scenario value | `-1`; MultiMissile 200, 175, 150, 125 |
| `SW.Animation` | A | AnimationType | by Type |
| `SW.AnimationHeight` | A | integer | by Type |
| `SW.AnimationVisibility` | A | none, owner, allies, team, enemies, all | not stated |
| `SW.Sound`, `SW.ActivationSound` | A | sound | by Type |
| `SW.CreateRadarEvent` | A | boolean | `no` |
| `SW.ShowCameo` | A | boolean | `yes` |
| `SW.TimerVisibility` | A | none, owner, allies, team, enemies, all | `all` |
| `TabIndex` | P | integer 0 to 3 | `1` (invalid values log and use `1`) |

### Keys that belong to one `Type=`

| Type | Keys (all Ares) |
|---|---|
| MultiMissile | `Nuke.Payload` (WeaponType, default `NukePayload`), `Nuke.TakeOff` (Animation, default `[General] NukeTakeOff`), `Nuke.PsiWarning` (Animation, default `PSIWARN`), `Nuke.SiloLaunch` (boolean, default `yes`) |
| LightningStorm | `Lightning.Duration`, `.RadarOutage`, `.HitDelay`, `.ScatterDelay`, `.ScatterCount` (default 1), `.Separation`, `.DebrisMin` (2), `.DebrisMax` (4), `.CloudHeight` (leptons, -1), `.RadarOutageAffects` (house enumeration, `enemies`), `.PrintText`, `.IgnoreLightningRod`, `.BoltExplosion`, `.Sounds`, `.Clouds`, `.Bolts`, `.Debris`; unset values take the matching `[General]` or `[AudioVisual]` key (Behaviour lists them) |
| PsychicDominator | `Dominator.FirstAnim`, `.FirstAnimHeight` (750), `.SecondAnim`, `.SecondAnimHeight` (0), `.FireAtPercentage`, `.ControlAnim`, `.Capture` (yes), `.Ripple` (yes), `.CaptureMindControlled` (yes), `.CapturePermaMindControlled` (yes), `.CaptureImmuneToPsionics` (no), `.PermanentCapture` (yes) |
| ChronoSphere | `Chronosphere.BlastSrc`, `.BlastDest`, `.ReconsiderBuildings` (yes), `.KillOrganic` (yes), `.KillTeleporters` (no), `.AffectsIronCurtain` (no), `.AffectsUnwarpable` (yes), `.AffectsUndeployable` (no), `.BlowUnplaceable` (yes), `.KillCargo` (no) |
| IronCurtain, ForceShield | `Protect.Duration`, `Protect.PowerOutage`, `Protect.PlayFadeSoundTime` (frames; ForceShield defaults come from `[General] ForceShield*`, IronCurtain from `[CombatDamage] IronCurtainDuration`, outage and fade time 0) |
| GeneticConverter | `Mutate.Explosion` (default `[General] MutateExplosion`), `Mutate.IgnoreCyborg` (no), `Mutate.IgnoreNotHuman` (no), `Mutate.KillNatural` (yes) |
| ParaDrop, AmerParaDrop | `ParaDrop.Types` (infantry and vehicle types), `ParaDrop.Num` (integers), `ParaDrop.Aircraft`, `ParaDrop.Count` (1), and the prefixed forms `ParaDrop.<Country or Side>.Plane<N>.Types/Num/Aircraft` |
| SpyPlane | `SpyPlane.Type` (AircraftType, `SPYP`), `SpyPlane.Count` (1), `SpyPlane.Mission` (mission, `SpyPlane Approach`) |
| DropPod | `DropPod.Types` (infantry, default `[General] DropPodTypes`), `DropPod.Veterancy` (float, 2.0), `DropPod.Minimum`, `DropPod.Maximum` (defaults `[General] DropPodMinimum/Maximum`) |
| UnitDelivery | `Deliver.Types` (list of TechnoTypes), `Deliver.Owner` (invoker, neutral, special or civilian; default invoker), `Deliver.BaseNormal` (yes) |
| SonarPulse | `SonarPulse.Delay` (frames, 60) |
| HunterSeeker | `HunterSeeker.Buildings` (default `[SpecialWeapons] HSBuilding`), `HunterSeeker.Type` (VehicleType, the side's), `HunterSeeker.RandomOnly` (no) |
| EMPulse | `EMPulse.Cannons` (BuildingTypes), `EMPulse.TargetSelf` (no), `EMPulse.Linked` (no), `EMPulse.PulseBall` (Animation, `PULSBALL`, `none` allowed), `EMPulse.PulseDelay` (frames, 32); Phobos adds `EMPulse.WeaponIndex` (integer, 0) and `EMPulse.SuspendOthers` (boolean, no) |
| Battery | `Battery.Power` (integer, 0), `Battery.Overpower` (BuildingTypes), `Battery.KeepOnline` (BuildingTypes) |

### Other sections

| Key | Section | Src | Type | Default |
|---|---|---|---|---|
| `DesignatorRange` | TechnoType | A | integer cells | the type's `Sight` |
| `InhibitorRange` | TechnoType | A | integer cells | the type's `Sight` |
| `HunterSeeker.Ignore` | TechnoType | A | boolean | `no` |
| `HunterSeeker.DetonateProximity`, `.DescendProximity`, `.AscentSpeed`, `.DescentSpeed`, `.EmergeSpeed` | VehicleType | A | integer | the `[General] HunterSeeker*` value |
| `HunterSeekerDetonateProximity`, `HunterSeekerDescendProximity`, `HunterSeekerAscentSpeed`, `HunterSeekerDescentSpeed`, `HunterSeekerEmergeSpeed` | `[General]` | A | integer | `0` (Tiberian Sun used 150, 700, 40, 50, 6) |
| `DropPodTypes`, `DropPodMinimum`, `DropPodMaximum` | `[General]` | A | infantry list, integer, integer | none, `0`, `0` |
| `HSBuilding` | `[SpecialWeapons]` | A | list of BuildingTypes | none |
| `<Name>=Frame,Count,Interval,MiniFrame,MiniCount,HotSpotX,HotSpotY` | `[MouseCursors]` | A | named cursor | the stock cursors exist without entries |
| Superweapon EVA lines | `evamd.ini` | A | `[EVA_<name>]` entries | the engine's table in `code/vox.cpp` |

The engine already reads `HSBuilding`, `DropPodInfantryMinimum/Maximum` and the stock globals named in Behaviour (lightning, dominator, force shield, iron curtain, chrono, mutator, reveal, `ChargeToDrainRatio`). It does not read `DropPodTypes`, `DropPodMinimum`, `DropPodMaximum`, the `HunterSeeker*` globals, `DesignatorRange`, `InhibitorRange` or `[MouseCursors]`.

## Behaviour

Markers: `[A]` is Ares documentation only, `[P]` is Phobos code, `[E]` is how the OpenYR engine works today.

### Firing path and synchronisation

- `[E]` A human shot is a `SPECIAL_PLACE` event carrying the superweapon index and a cell. `EventClass::Execute` (`code/event.cpp`) calls `HouseClass::Place_Special_Blast`, which calls `SuperClass::Discharged` and then `SuperClass::Place`. A computer house and any auto-fired weapon use the same call. Everything that changes game state has to sit on this path so every client runs it, and may draw random numbers only through the synchronised generator (`Random_Pick`, `Random_Double`, as the `Pick_*_Target` functions in `code/house.cpp` do).
- `[E]` The cursor, EVA lines, messages and cameo captions are local. A client can send any cell, so each check the cursor makes (range, designator, inhibitor, target kind, funds, charge) must be repeated in `Discharged`/`Place`, where all clients run it.
- `[P]` All Phobos effects run once per launch, after the Ares launch code: limbo delivery, limbo kill, detonation, `SW.Next`, conversion, `SW.Link`, EMPulse tagging, activation message, activation EVA, then the shot counter, then the Phobos trigger events 77 and 75 (those belong to the trigger-event spec). A superweapon started by `SW.Next` goes through the same code, so its own `SW.Next`, `SW.Link` and so on run as well.

### Availability

- `[A]` A superweapon whose restrictions fail behaves as if the building did not provide it: no cameo, no detection EVA, no charge. The building still plays its Super animation. `DisableableFromShell` still reads only the building's `SuperWeapon=` entry, and the building stays unbuildable when the lobby disables superweapons.
- `[P]` The order of checks (this is Phobos replacing the Ares check, so it is the order mods written for Phobos see): `SW.TechLevel` against the house tech level (engine: `Control.TechLevel` of `HouseClass`); `SW.Shots` against the house's shot count for this type; `SW.AllowPlayer` or `SW.AllowAI` by who controls the house; `SW.RequiredHouses` then `SW.ForbiddenHouses` by country; the single `AuxBuilding`; any one of `SW.AuxBuildings`; none of `SW.NegBuildings`; any one of `SW.AuxTechnos`; none of `SW.NegTechnos`. A failed check ends the evaluation. `SW.TechLevel=-1` has no special meaning (every house meets it), and the default `0` always passes.
- `[P]` "Owns" in the five aux and neg lists counts objects in limbo as well as objects on the map (passengers, absorbed infantry, limbo-delivered buildings), except types marked `Insignificant` or `DontScore`. A building type that is an upgrade counts through the number of upgrades of that type the house has installed, so upgrades satisfy these lists. The Ares page says the opposite (upgrades never satisfy `AuxBuilding`, `SW.AuxBuildings` or `SW.NegBuildings`); open question 2 covers it.
- `[A]` `AuxBuilding` also gates a superweapon that comes from a building upgrade. A superweapon that uses any aux or neg list does not support building animations.
- `[A]` `SW.Shots` counts every launch of that type by the house, including weapons granted by crates or map actions. Charge-drain weapons are not supported. `[P]` The count is kept per house and per type, and every launch increments it, including one started by `SW.Next`.
- `[A]` `SW.AlwaysGranted=yes` makes the weapon available to every house that is not defeated, with no building. It still obeys `AuxBuilding`, `SW.AuxBuildings`, `SW.NegBuildings`, `SW.RequiredHouses`, `SW.ForbiddenHouses`, `SW.AllowPlayer` and `SW.AllowAI`.
- `[A]` `[BuildingType] SuperWeapons=` grants more weapons than `SuperWeapon` and `SuperWeapon2`. Each entry is checked on its own. The first available entry sets the EVA line played when the building is placed and drives the building's charge animations. `SuperWeapon` and `SuperWeapon2` stay special and not every part of the logic covers the list.
- `[A]` Building charge animations (charging, one minute left, ready, fired) work for any weapon type, not only the stock ones. `[E]` `BuildingClass::Update_Super_Anims` (`code/building.cpp`) reads `Class->SuperWeapon` only, so `SuperWeapon2` and the new list need adding there.
- `[A]` `SW.InitialReady=yes`: the first weapon of this type is ready as soon as it becomes available, then recharges normally. `[P]` Phobos also uses it as the default for `SW.Link.Ready` on a granted weapon, and suppresses a building's charged-state animation until the weapon's timer has started once unless the key is `yes`.
- `[A]` `SW.VirtualCharge=yes`: after the weapon first becomes available it keeps charging when the last providing building is lost (in the background or off the map). The cameo returns with the charge kept. Not supported with `UseChargeDrain=yes`.
- `[A]` `SW.Group` is a plain integer that tells apart several weapons of one `Type=`. Its only documented use is matching a team script action for the Iron Curtain.
- `[A]` `Type=` and `Action=` cannot be changed from game-mode INI files or maps. A mod adds a new weapon section instead.

### Targeting and range

- `[A]` `SW.Range`: one float is a circle radius in cells (not limited to the 10 of `CellSpread`); two integers are a rectangle width and height. The Lightning Storm page says a single value is the diameter, which contradicts the general page (open question 3). Defaults by Type are in the table below.
- `[A]` `SW.AffectsHouse` is relative to the firing house; `team` is `owner,allies`, `all` is `owner,allies,enemies`. `SW.AffectsTarget`: `land` and `water` filter the cell; if neither is listed both pass. The object kinds filter what is hit; if none is listed everything passes; `none` means no restriction. What `empty` selects is not documented (open question 4).
- `[A]` `SW.RangeMaximum` and `SW.RangeMinimum` measure cells from a providing building. The weapon can fire at a cell if at least one providing building satisfies both limits; otherwise the player sees `NoCursor`. A negative value takes the Type default, usually no check. `[P]` Phobos measures the squared straight-line distance between the target cell and the building's centre cell, and a building only counts if it is alive, has health, is out of limbo and is powered on.
- `[A]` A designator is any of the player's own units or structures of a listed type; one must be within its `DesignatorRange` of the target cell for the weapon to fire there, else `NoCursor`. `SW.AnyDesignator=yes` accepts any type and overrides the list. An empty list needs no designator.
- `[A]` An inhibitor is an enemy unit or structure of a listed type within its `InhibitorRange` of the target. One in range gives `NoCursor` and stops the launch. A building inhibits only while online (power and `NeedsEngineer`). `[P]` Phobos treats any object not allied to the firer as an inhibitor, requires only power for buildings (no engineer check), and skips deactivated objects.
- `[A]` `SW.FireIntoShroud=no` stops a human firing into unexplored shroud. `SW.RequiresTarget` and `SW.RequiresHouse` restrict where a human can click; elsewhere shows `NoCursor`. Setting them against `SW.AffectsTarget` so that nothing can be affected is allowed and wastes the shot.
- `[A]` `SW.UseAITargeting=yes`: clicking the cameo fires at an automatically chosen cell with no manual targeting. The cameo is darkened while the AI constraints fail, and clicking anyway shows `Message.CannotFire`. `SW.AutoFire=yes` fires the weapon as soon as it is ready, even for humans, at the AI-chosen target. `SW.ManualFire=no` removes the manual cursor; the page says `SW.ManualFire` and `SW.ShowCameo` are ignored while `SW.AutoFire=no`, which is the default.
- `[A]` `SW.AITargeting` modes: None (never fires), Nuke, LightningStorm, PsychicDominator, GeneticMutator, IronCurtain (waits for a team script with a matching `SW.Group`), ForceShield (the last launch of a weapon with `AIDefendAgainst=yes`), ParaDrop (5 by 5 free area near the favourite enemy's base centre, else the owner's), DropPod (random land cell in one of four outer sectors around the owner's base), Offensive, MultiMissile (summed `ThreatPosed` around buildings, random 0 to 100 for cloaked ones), HunterSeeker (no target, needs a favourite enemy), NoTarget, Stealth (cloaked targets only), Self (a powered own building that provides the weapon), Base, EnemyBase, Attack (as NoTarget when a building was attacked recently), LowPower, LowPowerAttack, LightningRandom (random cell). The Nuke, LightningStorm, Offensive and Stealth modes use the Ion Cannon rating, picking at random among the top-rated objects.
- `[A]` Defaults per mode (`SW.AIRequiresTarget`, `SW.AIRequiresHouse`, constraints, preference): Nuke infantry,units,buildings / enemies / enemy / offensive; LightningStorm the same with constraints enemy and `lightningstorm_inactive`; PsychicDominator infantry,units / all / enemy,dominator_inactive,offensive_cell_clear / none; GeneticMutator infantry / all / offensive_cell_clear; IronCurtain none of the three set; ForceShield preference defensive; ParaDrop preference offensive; DropPod constraint enemy; Offensive infantry,units,buildings / enemies / enemy; MultiMissile buildings / none / enemy / offensive; Stealth infantry,units,buildings / enemies; EnemyBase constraint enemy; Self land,water or none only. Setting `SW.AIRequiresTarget` or `SW.AIRequiresHouse` on a mode marked "do not use" can stop the weapon firing.
- `[A]` Constraints: `None`, `Enemy` (needs a favourite enemy), `Offensive_Cell_Set/Clear` (map action 135 sets the cell, 136 clears it), `Defensive_Cell_Set/Clear` (140 sets, 141 clears), `LightningStorm_Inactive`, `Dominator_Inactive`, `Attacked`, `LowPower`. The last four do not apply to a human firing by hand. Preference `Offensive` or `Defensive` replaces the target with that cell while it is set. A preferred target type set by map action 35 other than 1 is not supported, and range, designator and inhibitor checks may be skipped for it. Base centre is set and cleared by map actions 137 and 138. The range, designator and inhibitor checks apply to every mode except IronCurtain, HunterSeeker and NoTarget. The docs give no defaults table row for Attack, LowPower and LowPowerAttack.
- `[E]` The engine picks AI targets in `HouseClass::AI_Super_Weapons` (`code/house.cpp`) by `Type=` with fixed rules: `Pick_Strike_Target`, `Pick_Ion_Cannon_Target`, `Pick_Drop_Target`, `Pick_Dominator_Target`, `Pick_Mutator_Target`, `AI_Chem_Missile`, `AI_Drop_Pods`, `AI_Hunter_Seeker`, and the defensive-cell logic for the force shield. `PreferredTargetCell`, `PreferredDefensiveCell` and `PreferredDefensiveCell2` already hold the trigger-set cells.
- `[E]` Today `ScrollClass::What_Action` (`code/scroll.cpp`) asks `SuperWeaponTypeClass::What_Action` for the targeting cursor and `Super_Weapon_Mouse` (`code/display.cpp`) maps the action to a fixed mouse shape. Only EMPulse has a range test. `DisplayClass::Mouse_Left_Release` finds the weapon to fire with `SuperWeaponTypeClass::From_Action`, which returns the first declared weapon with that `Action=`. Two weapons sharing an `Action=` therefore always fire the earlier one (open question 1).

| Type | `SW.Range` default | `SW.AffectsHouse` | `SW.AffectsTarget` | `SW.AITargeting` | `Cursor` |
|---|---|---|---|---|---|
| LightningStorm | `[General] LightningCellSpread` | all | all | LightningStorm | LightningStorm |
| MultiMissile | not used | all | all | Nuke | Nuke |
| PsychicDominator | `[General] DominatorCaptureRange` | all | infantry,units | PsychicDominator | PsychicDominator |
| ChronoSphere | 3,3 | all | infantry,units | none (AI cannot use) | Chronosphere |
| ChronoWarp | only draws the range lines | | | none | Chronosphere |
| IronCurtain | 3,3 | all | all | IronCurtain | IronCurtain |
| ForceShield | `[General] ForceShieldRadius` | team | buildings | ForceShield | ForceShield (`NoCursor` NoForceShield) |
| GeneticConverter | 3,3 (ignored with `Mutate.Explosion`) | all | all | GeneticMutator | GeneticMutator |
| ParaDrop, AmerParaDrop | | | | ParaDrop | ParaDrop |
| SpyPlane | | | | ParaDrop | SpyPlane |
| PsychicReveal | `[CombatDamage] PsychicRevealRadius`, capped at 10 unless set; `-1` is the whole map | | | ParaDrop | PsychicReveal |
| SonarPulse | 10; `-1` is the whole map | enemies | water | Stealth | |
| GenericWarhead | | | | Offensive | |
| UnitDelivery | | | | ParaDrop | |
| DropPod | | | | DropPod | ParaDrop |
| HunterSeeker | | enemies | all | HunterSeeker | |
| EMPulse | | | | None | Attack (`NoCursor` AttackOutOfRange) |
| Battery | | | | LowPower | |

- `[A]` A whole-map `SW.Range=-1` on PsychicReveal and SonarPulse fires from the cameo click with no target selection.

### Charge, cost and charge-drain

- `[A]` `Money.Amount` is added to the firing house when the weapon fires; a negative value takes credits. With too few credits the launch is aborted, `EVA.InsufficientFunds` plays and `Message.InsufficientFunds` shows. The cost shows in the cameo tooltip and the cameo is greyed while the house cannot pay. `[P]` The Phobos affordability test is `HouseClass::CanTransactMoney(amount)`.
- `[A]` `Money.DrainAmount` is added to the owner each time `Money.DrainDelay` frames pass while a `UseChargeDrain=yes` weapon is active.
- `[A]` `SW.ChargeToDrainRatio` multiplies the recharge time to give the active time of a charge-drain weapon (it must not be 0). `SW.Unstoppable=yes` makes a click on the active cameo do nothing. Ares forces `UseChargeDrain` on for `Type=Firestorm`, `Type=Battery` also uses the charge-drain logic, and the docs warn that other Types behave unexpectedly with it. `[E]` The engine already runs the drain cycle in `SuperClass::Discharged` using the global `Rule->ChargeToDrainRatio`.
- `[A]` `SW.Deferment` is the number of frames between firing and the effect. Defaults: LightningStorm `[General] LightningDeferment`, PsychicDominator 0, UnitDelivery 20. Not every Type supports it. `Message.Activate` and `SW.ActivationSound` happen at the end of the deferment.
- `[A]` `Message.Abort` shows when a launch is refused because another weapon of the same kind is active (Lightning Storm, Psychic Dominator). `[E]` The engine refuses in `SuperClass::Discharged` and prints `Print_Refusal` text; a refused shot keeps its charge.

### Effect and warhead

- `[A]` `SW.Warhead` and `SW.Damage` defaults by Type: LightningStorm `[General] LightningWarhead` and `LightningDamage`; PsychicDominator `DominatorWarhead` and `DominatorDamage` (nothing dealt at 0 or below); GeneticConverter `[SpecialWeapons] MutateWarhead` (or `MutateExplosionWarhead` with `Mutate.Explosion=yes`) and 10000; MultiMissile empty and `-1`, meaning the payload weapon's own warhead and damage; GenericWarhead has no default.
- `[A]` `Type=GenericWarhead` detonates the warhead in the target cell, not on an object, so the warhead needs a `CellSpread` to reach infantry. The firing building needs `DamageSelf=yes` to be hurt.
- `[A]` `SW.CreateRadarEvent=yes` gives every player a radar event rectangle on the target cell.
- `[P]` `Detonate.Weapon` wins when both it and `Detonate.Warhead` are set. With a weapon the detonation uses `Detonate.Damage` or the weapon's `Damage`. With a warhead `Detonate.Warhead.Full=yes` runs a full dummy detonation (animations and per-target effects); `no` only deals area damage and applies Phobos warhead effects. A warhead with no `Detonate.Damage` deals 0. The detonation is credited to the first building of the house that provides this weapon, is alive, powered and in range (`SW.RangeMinimum/Maximum`), taken in the house's building order. If none qualifies, the house is credited without a building. The Phobos page says the detonation "respects" the range limits; the code uses them only to find the credited building, never to refuse the detonation. `Detonate.AtFirer=yes` detonates at that building's centre and does nothing if no building qualifies. A target cell outside the map logs a message and does nothing.
- `[P]` `SW.Next`: for each launched entry the weapon is the firing house's own copy of that type. With `SW.Next.RealLaunch=yes` the copy must be present, ready, not suspended, and the house must be able to pay its `Money.Amount`; entries that fail are skipped one by one (the Phobos page says the house "must own all" of them, which the code does not enforce). The copy then needs, unless the ignore keys say otherwise, no enemy inhibitor in range (`SW.Next.IgnoreInhibitors=no` by default) and a designator in range (`SW.Next.IgnoreDesignators=yes` by default skips the designator test). It fires at the same cell as the first weapon, then its charge restarts. With `RealLaunch=no` the entry always fires (present or not, ready or not, funded or not) and the code puts the timer values back after the shot; the ready flag is left cleared (open question 16). Random mode: see "Rolls and weights".
- `[P]` `SW.Link` acts on the firing house when the weapon fires, unless the house is defeated. For each chosen entry: `SW.Link.Grant=yes` adds the weapon as a one-time weapon (like the nuke crate) when the house does not hold it. Then, in order: `SW.Link.Reset=yes` restarts the charge of a held weapon; else `SW.Link.Ready=yes`, or a freshly granted weapon whose own `SW.InitialReady=yes`, makes it fully charged at once; else a freshly granted weapon starts charging from zero. A held weapon with neither key set is left alone. The local player gets `Message.LinkedSWAcquired` and `EVA.LinkedSWAcquired` when at least one entry was granted, reset or readied. A granted cameo is added to the sidebar for the local player. Because the grant is one-time, the granted weapon disappears after it fires.
- `[P]` `LimboDelivery.*`: for each chosen type a building is created for the firing house, not placed on the map, and registered as owned: it counts for prerequisites, power, the construction yard and secret lab lists, and the aux checks above, and it feeds factory plants. The Phobos page lists these flags as confirmed to work: `FactoryPlant`, `OrePurifier`, `SpySat`, `KeepAlive`, `Prerequisite*`, `GenericPrerequisites`, `SuperWeapon`, `SuperWeapon2`, `SuperWeapons`, `SW.AuxBuildings`, `SW.NegBuildings`; other building flags are not guaranteed, and a type used with an AI trigger condition needs `LegalTarget=yes`. A type with `BuildLimit` above 0 is skipped when the house already owns that many (the undeploy-into type counts too); other limits are not checked. Each building gets the next ID of `LimboDelivery.IDs`; a building past the end of the ID list keeps the last ID used (`-1` if the list is empty). The building is discovered for the owner, and for the current player in a campaign. It cannot be selected or attacked. A building with `AutoDeath` settings is registered for them.
- `[P]` `LimboKill.*`: for every house matching `LimboKill.AffectsHouse` (relative to the firer), and for each ID in the list in order, up to `LimboKill.Counts[i]` limbo-delivered buildings with that ID are removed (all of them when the count is missing; a repeated ID is processed once, at its first position). A removed building is stunned, taken out of limbo tracking and destroyed with no death animation, no explosion and no credited killer. The Phobos page gives `LimboKill` as the only way to remove them from the game. A delivered building needs `KeepAlive=no` or a `LimboKill` weapon, or the game may never end.
- `[P]` `Convert.*` converts every object of the listed types owned by an eligible house, including passengers and garrisoned units, once at launch; see `phobos-autodeath-convert-animunit.md`. Buildings are not supported.

### Rolls and weights (`LimboDelivery`, `SW.Next`, `SW.Link`)

- `[P]` With no `RandomWeights*` list the whole list applies in order. With any weights list, each "roll" picks one entry: roll `i` succeeds with probability `RollChances[i]` (a missing list gives one roll that always succeeds), then picks an entry by the weights of list `i`, or by the last list when there are more rolls than lists. A weight of 0 never picks. A picked index beyond the type list is dropped. The rolls use the synchronised scenario random generator, so they are deterministic across clients if made in the launch path. Chances are compared as fractions from 0 to 1, so the INI value `25%` reads as 0.25.

### Delivery Types `[A]`

- MultiMissile: the missile uses its own section's `Nuke.Payload` weapon (Ares respects each weapon's `WeaponType`, where Yuri's Revenge used the first nuke's); the warhead and damage of the strike come from `SW.Warhead` and `SW.Damage` or, when unset, from the payload. `Nuke.SiloLaunch=no` creates the payload off-screen aiming at the target instead of launching from a building with `NukeSilo=yes`. Several silo types may provide one weapon. A `Vertical=no` projectile flies straight at the target as in Tiberian Sun. The `NUKEBALL` impact animation for the `NUKE` warhead is set through `PreImpactAnim`. `SW.ActivationSound` defaults to `[AudioVisual] DigSound`. `[E]` `SuperClass::Place` looks for the first `NukeSilo=yes` type naming the weapon and stores the behaviour number in `LastSuperWeaponIndex`; `BuildingClass::Do_MISSION_MISSILE` then launches the `WeaponType` of the section at that list position, so the weapon that ordered the launch does not choose the missile. The payload is the `NukePayload` section (`code/bullet.cpp`).
- LightningStorm: a storm lasts `Lightning.Duration` frames (`-1` forever). Clouds appear over the target every `Lightning.HitDelay` frames and over `Lightning.ScatterCount` random cells in range every `Lightning.ScatterDelay` frames; 0 or below disables each. `Lightning.Separation` is the least sum of x and y cell differences between scatter clouds. `SW.Range` sets the strike area; radar is jammed for `Lightning.RadarOutage` frames for the houses in `Lightning.RadarOutageAffects`. Lightning rods attract strikes unless `Lightning.IgnoreLightningRod=yes`. An empty `Lightning.Clouds` list disables the weapon; an empty `Lightning.Bolts` list still deals damage. Do not put `Bouncer=yes` animations in `Lightning.Bolts`. Defaults come from `[General]` `LightningStormDuration`, `LightningHitDelay`, `LightningScatterDelay`, `LightningSeparation`, `LightningPrintText`, `WeatherConBoltExplosion`, `WeatherConClouds`, `WeatherConBolts`, `MetallicDebris`, `[AudioVisual]` `StormSound`, `LightningSounds` and the scenario `[Lighting] Ion*` values. `[E]` `LightningStormClass` (`code/lstorm.cpp`) is a static singleton with one active storm and reads those globals directly; the per-weapon values need to be captured when it starts.
- PsychicDominator: `Dominator.FirstAnim` hovers, the blast fires after `Dominator.FireAtPercentage` percent of its frames, then `Dominator.SecondAnim` plays. `SW.Range` is the capture area and `SW.Damage` with `SW.Warhead` is dealt first (nothing at 0 or below). The `Dominator.Capture*` switches decide who is taken; `Dominator.PermanentCapture=no` lets other controllers retake the victim. `[E]` `PsychicDominatorClass` (`code/psydom.cpp`) is also a singleton, reads the `Dominator*` globals and uses `Can_Be_Dominated`.
- ChronoSphere: needs a ChronoWarp weapon (the first in the list, or `SW.PostDependent`). The sphere picks units, the warp picks the destination; only the sphere's `Chronosphere.*` keys act. Organic units die unless `Chronosphere.KillOrganic=no`; `Teleporter=yes` units are shifted unless `KillTeleporters=yes`; iron-curtained and `Warpable=no` units are skipped or taken according to the two `Affects*` keys. Buildings can be shifted: a building that does not fit blows up at the source (`BlowUnplaceable=yes`) or stays; vehicle-type buildings look for a spot like vehicles. Known issues from the docs: build-up animations restart and turret facing resets. `[E]` `SuperClass::Chrono_Warp` and `Chrono_Shift` hard-code the 3 by 3 area, kill organics, and skip iron-curtained units; it moves feet only. The chronoshift flags `Chronoshift.Allow`, `Chronoshift.Crushable` are in `chronoshift.md`.
- IronCurtain, ForceShield: `Protect.Duration` frames of protection; ForceShield also blacks out the owner's power for `Protect.PowerOutage` frames and plays `SpecialSound` `Protect.PlayFadeSoundTime` frames before the end (must be below the duration). The colour of the protected building is fixed per Type. `[E]` `SuperClass::Iron_Curtain` covers a 3 by 3 block and `Force_Shield` covers `ForceShieldRadius` around the cell.
- GeneticConverter: with `Mutate.Explosion=yes` one blast of `SW.Damage` through `SW.Warhead` ignores the other `Mutate.*` keys. With `no` every infantryman in range is damaged through the warhead (verses and immunities apply); `Mutate.IgnoreCyborg` and `IgnoreNotHuman` spare those types and `KillNatural=yes` kills `Natural=yes` infantry outright. `[E]` `SuperClass::Genetic_Mutator` uses the global `MutateExplosion` and a fixed 3 by 3 area.
- ParaDrop, AmerParaDrop: `ParaDrop.Types` and `ParaDrop.Num` take vehicles as well as infantry (stock lists convert a vehicle into an invisible infantry). `ParaDrop.Count` planes are sent. Each plane reads its aircraft, types and numbers from the first of nine places that defines them: `ParaDrop.<Country>.Plane<N>.*`, `ParaDrop.<Side>.Plane<N>.*`, `ParaDrop.Plane<N>.*`, `ParaDrop.<Country>.*`, `ParaDrop.<Side>.*`, `ParaDrop.*` on the weapon, then `ParaDrop.*` on the country, on the side, and the `[General]` lists. Plane 1 uses no `Plane<N>` part. Types and Num must be set together; the aircraft is read separately from them. `[E]` `SuperClass::Paradrop` reads the side lists only and sends the hard-coded `PDPLANE`.
- SpyPlane: `SpyPlane.Count` aircraft of `SpyPlane.Type` on `SpyPlane.Mission`. `[E]` `SuperClass::Paradrop` sends one `SPYP` per `AllyParaDropInf` entry.
- DropPod: between `DropPod.Minimum` and `DropPod.Maximum` pods, each unit picked at random from `DropPod.Types` (repeats raise the odds), with `DropPod.Veterancy` (2.0 is elite) unless the unit already ranks higher. Placement retries at most 3 times the unit count and then stops. No default types exist. `[E]` `SuperClass::Drop_Pods` hard-codes `E1` and `E2`, elite, and `DropPodInfantryMinimum/Maximum`.
- UnitDelivery: after `SW.Deferment` (20) all `Deliver.Types` appear at once near the target, on the ground (aircraft land, flyers take off), owned by `Deliver.Owner`, with infantry squads grouped in a cell. Placement order is not fixed and uses random numbers. AI-owned units hunt, buildings guard and human-owned units guard area. Naval types go where they can exist. `Deliver.BaseNormal=no` stops delivered buildings giving base space. No engine counterpart exists.
- SonarPulse: any cloaked object in range (or on the map) decloaks for `SonarPulse.Delay` frames, including ones under a cloak generator. No engine counterpart exists.
- HunterSeeker: up to `SW.MaxCount` buildings from `HunterSeeker.Buildings` each launch one `HunterSeeker.Type` (default the side's); if the house owns none the weapon discharges without launching. `HunterSeeker.RandomOnly=no` prefers non-civilian targets for humans in multiplayer. Seekers ignore iron-curtained and warping objects and any type with `HunterSeeker.Ignore=yes`. `[E]` `SuperClass::Place` launches one seeker from the last matching building of the house.
- EMPulse: up to `SW.MaxCount` cannons fire together at the target or at themselves. A cannon is a building of `EMPulse.Cannons` (or any `EMPulseCannon=yes` building when the list is empty) that has a primary weapon, is powered and is in range. `SW.RangeMaximum` below 0 uses the cannon's primary weapon range, `SW.RangeMinimum` below 0 its minimum range. A `EMPulseCannon=yes` cannon turns, plays `EMPulse.PulseBall`, waits `EMPulse.PulseDelay` frames and throws one bullet with its primary weapon. `EMPulse.TargetSelf=yes` detonates the primary weapon once on each cannon at once (it hurts the cannon only with `DamageSelf=yes`). A cannon without `EMPulseCannon=yes` fires its primary weapon at the target with no charge, turning or range check. `EMPulse.Linked=yes` needs only one cannon in range. `[E]` Stock behaviour is in `SuperClass::Place` (`SUPER_EM_PULSE`) and `BuildingClass::Do_MISSION_MISSILE`: the nearest powered cannon only, one target per house in `HouseClass::EMPDest`, hard-coded `PULSBALL` and 32-frame delay.
- Battery: while active (charge-drain), the owner gets `Battery.Power` extra power independent of base damage, the types in `Battery.Overpower` are overpowered (secondary weapon, no `Overpowerable=yes` needed) and the types in `Battery.KeepOnline` stay powered. The AI uses it on low power.
- Firestorm: Ares forces charge-drain and ignores the assigned `Action=`; the AI never uses it, and map actions 92 and 93 control it.

### Phobos EMPulse additions `[P]`

- `EMPulse.WeaponIndex` picks the weapon slot a cannon fires (not used with `EMPulse.TargetSelf=yes`). A negative index selects the weapon the building would use against the target cell. If another `EMPulse` weapon with a different index tags the same cannon before it fires, the later one wins. Range defaults still read weapon slot 0 unless `SW.RangeMinimum/Maximum` are set.
- When a launch tags cannons, Phobos picks them from the house's building list in list order (not nearest first), up to `SW.MaxCount`; a `SW.MaxCount` of 0 tags every eligible cannon in Phobos.
- `EMPulse.SuspendOthers=yes` suspends every other EMPulse weapon of the house that shares at least one cannon type with this one (two empty cannon lists count as sharing). The suspension ends when the first tagged cannon starts its missile mission. See open question 14 for a suspected indexing bug.

### Presentation

- `[A]` `Cursor` is used where the weapon may fire; `NoCursor` where target type, owner, range or a designator forbids it. Values are names from `[MouseCursors]` or the stock list; a definition is seven comma-separated values (frame, count, interval, minimap frame, minimap count, hot spot x as Left, Center or Right, hot spot y as Top, Middle or Bottom). `[E]` There is no cursor table; `Super_Weapon_Mouse` returns a `MouseType` from the action. A name-to-shape table is a separate feature that these keys depend on.
- `[A]` EVA keys: `EVA.Detected` plays to other players when the building is built, `EVA.Ready` to the owner when ready, `EVA.Activated` when fired, `EVA.Impatient` on an early click, `EVA.SelectTarget` when a target is needed, `EVA.InsufficientFunds` when the launch is refused. `none` disables a line. `[E]` Ready and fired lines come from `Speak_Ready` and `Speak_Eva` calls inside `SuperClass`; detection is announced in `BuildingClass::Place` for six stock behaviours.
- `[A]` Messages: `Message.Detected`, `.Launch` and `.Activate` go to every player; `.Ready` and `.Abort` go to the owner; `Message.CannotFire` goes to the owner when AI targeting finds nothing. `Message.FirerColor=yes` uses the firing house's colour, else `Message.Color` if set, else the player's.
- `[P]` The recipient keys pick one line per client by relation to the activating house: the activating house when the local player controls it (owner), else an ally of the local player (allies), else enemies. A line left empty prints or plays nothing. The message uses the activating house's colour; the EVA plays with no position. They run in addition to the Ares `Message.Launch` and `EVA.Activated`, so both can appear.
- `[A]` Cameo captions: `Text.Hold` while powered but unable to charge, `Text.Ready` when charged, `Text.Charging` and `Text.Active` for charge-drain weapons, `Text.Preparing` otherwise.
- `[A]` Lighting: `Light.*` only works for LightningStorm, MultiMissile and PsychicDominator. `-1` keeps the scenario value. Too high an ambient value slows the game.
- `[A]` `SW.Animation` plays at the target at `SW.AnimationHeight` for the houses in `SW.AnimationVisibility`; `SW.Sound` plays at the target. Type defaults: ChronoSphere `[General] ChronoPlacement`, GeneticConverter `IonBlast` with sound `[AudioVisual] GeneticMutatorActivateSound`, IronCurtain `IronCurtainInvokeAnim`, ForceShield `ForceShieldInvokeAnim`, all at height 5; PsychicReveal sound `PsychicRevealActivateSound`.
- `[A]` `SW.ShowCameo=no` removes the cameo. `SW.TimerVisibility` limits who sees the timer of a `ShowTimer=yes` weapon (observers count as allied with everyone).
- `[P]` `TabIndex` puts the cameo on tab 0 (buildings), 1 (arsenal, the stock tab for superweapons), 2 (infantry) or 3 (vehicles). `[E]` `SidebarClass::Which_Column` returns column 1 for every `RTTI_SPECIAL`; the sort key is `CameoSortOrder`.
- `[A]` `SW.PostDependent` names the weapon fired right after this one; in Red Alert 2 only the ChronoSphere uses it.

### AI delay `[P]`

- `AISuperWeaponDelay` in `[General]`: when set, computer-controlled houses (not human, not neutral) run their superweapon check from the per-frame house update instead of the periodic expert pass. A positive value runs it at most once per that many frames; 0 or below runs it every frame. The check still needs `IQ` at or above `[IQ] SuperWeapons` in campaigns. When the key is unset the stock pass (106 to 112 frames) is used. The timer is per house and deterministic; no random numbers.
- `[E]` `HouseClass::Expert_AI` returns `TICKS_PER_SECOND * 7 + Random_Pick(1, TICKS_PER_SECOND / 2)`, which is 106 to 112 frames at 15 frames a second, and calls `AI_Super_Weapons` inside when `Session.Type != GAME_NORMAL || IQ >= Rule->IQSuperWeapons`.

### Interactions

- Ares and Phobos `Convert.*` on a superweapon change the type of objects owned by the listed houses; the `Chronoshift.*` flags and `IronCurtain`/`ForceShield` modifiers on techno types and warheads (separate specs) apply to the effects here.
- `[P]` A launch site must be alive, have health above 0, be out of limbo and be powered on; Phobos has no other test for owner change, transport, cloak or temporal effects on the building. Whether an EMP-disabled building counts as powered off is not shown in the source read (open question 17).
- `[P]` `SW.Link` does nothing for a defeated house. A house defeated before a deferred effect lands is not documented.

## What stock YR does

- A building grants one or two weapons through `SuperWeapon=` and `SuperWeapon2=`, gated by the single `AuxBuilding`. There is no per-country, per-controller, shot-count or tech-level restriction.
- Each hard-coded `Type=` has fixed areas and values: Iron Curtain, Genetic Mutator and the chronoshift cover the target cell and the eight around it; the force shield, lightning storm, dominator, psychic reveal and ion cannon read their globals; drop pods are `E1` and `E2`; paradrops use the side lists and `PDPLANE`; the spy plane sends one `SPYP` per ally paradrop entry.
- Firing costs nothing and cannot be refused for lack of funds, range, designators or inhibitors. The only refusals are a second lightning storm or dominator blast.
- The cursor for a weapon comes from its `Action=` through a fixed set of mouse shapes. A click fires the first declared weapon whose `Action=` matches.
- Captions, EVA lines and messages are fixed per behaviour. Cameos always sit on the second sidebar tab.
- A human fires by hand. A computer house uses one hard-coded target rule per behaviour in a pass that runs every 106 to 112 frames.
- Only the lightning storm, nuclear missile and dominator relight the map, through the global ion, nuke and dominator lighting values.
- Mission scripts and triggers can grant, charge and aim weapons (`PreferredTargetCell`, the defensive cells), which this feature keeps.

## Where it hooks in OpenYR

Already present: `SuperWeaponTypeClass` and `SuperClass` with the full stock behaviour; the hard-coded types in `Special_From_Name`; the grant and revoke passes `HouseClass::Update_Present_Super_Weapons` and `Enable_Available_Super_Weapons`; the per-`Type=` AI pickers; `Rule->ChargeToDrainRatio`; the charge-drain cycle; the two-click chronosphere pairing; `SidebarPCX` and `CameoSortOrder`; the global rules keys for lightning, dominator, force shield, iron curtain, chrono, mutator, paradrop and drop pods. Not present: any `SW.*` key, `Type=GenericWarhead`, `UnitDelivery`, `SonarPulse` and `Battery`, a per-weapon extension of effect values, named cursors, per-weapon EVA and message labels, shot counts, limbo-delivered buildings.

Build order, smallest first (steps 1 to 4 cover the keys that Mental Omega style mods use most; the order rests on the prefix counts in `RESEARCH_BRIEFING.md` from the owner's scan of Mental Omega — `SW.*`, `EVA.*`, `Message.*`, `Money.*`, `ParaDrop.*` rank high — because `ares-tags.md` gives no per-key counts; run `tools/mod_scan.py` to reorder within a step):

1. Parse only. Add the new fields to `SuperWeaponTypeClass` (`code/suprtype.h`), read them in `SuperWeaponTypeClass::Read_INI`, add them to `Serialize`, bump `SaveVersionInfo::REVISION` (`code/savever.h`), and add the ones that change play to `Compute_CRC`. Add `GenericWarhead`, `UnitDelivery`, `SonarPulse` and `Battery` to `Special_From_Name` and the `SuperWeaponType` enum (`code/super.hh`). Read `[BuildingType] SuperWeapons=` in `BuildingTypeClass::Read_INI` (`code/builtype.cpp`). No behaviour changes yet; mod coverage figures rise.
2. Fix the shared-`Action=` click: in `DisplayClass::Mouse_Left_Release` fire `SuperWeaponTypes[Map.IsTargettingMode]` (the weapon whose cameo was clicked) instead of `From_Action`. Every mod with more than one weapon on a stock action needs this before any other `SW.*` key is useful.
3. Presentation, the cheapest and most used: `EVA.*` and `Message.*` (new `SuperClass`/`HouseClass::Place_Special_Blast` call sites, print with `Session.Messages.Add_Message`, speak with `Speak_Eva`), the Phobos recipient keys and `Message/EVA.LinkedSWAcquired`, `Text.*` captions in `SidebarClass::StripClass::Draw_It` (`code/sidebar.cpp`), `SW.Animation/Sound/ActivationSound`, `SW.CreateRadarEvent`, `SW.ShowCameo`, `SW.TimerVisibility` (`code/tactical.cpp`), Phobos `TabIndex` in `SidebarClass::Which_Column`. Add a `[MouseCursors]` table and use it for `Cursor`/`NoCursor` in `Super_Weapon_Mouse`.
4. Availability: one function `HouseClass::Super_Weapon_Available(SuperClass const *)` called from `Update_Present_Super_Weapons` and `Enable_Available_Super_Weapons`, implementing the check order in Behaviour, with per-house `ShotCount` on `SuperClass`; then `SW.AlwaysGranted`, `SuperWeapons=`, `SW.InitialReady`, `SW.VirtualCharge`. Extend the building charge animation in `code/building.cpp` to the list.
5. Cost and effect values: `Money.Amount` and the drain keys (funds test in `Discharged`, tooltip in `Help_Text`, grey cameo in `Draw_It`), `SW.Deferment` (a saved list of pending effects), `SW.Range/AffectsHouse/AffectsTarget` replacing the fixed 3 by 3 loops in `SuperClass::Iron_Curtain`, `Genetic_Mutator`, `Chrono_Warp`, `Force_Shield`, `SW.Warhead/Damage`, and `Type=GenericWarhead` through the existing explosion-damage routine used by `Genetic_Mutator`. Add Phobos `Detonate.*` here; it needs only the damage routine and a firer search.
6. Range limits and targeting: `SW.RangeMinimum/Maximum`, designators and inhibitors (new `TechnoTypeClass` fields `DesignatorRange`, `InhibitorRange`), `SW.RequiresTarget/RequiresHouse/FireIntoShroud` in `SuperWeaponTypeClass::What_Action`; the same tests again in `SuperClass::Discharged`. Then `SW.UseAITargeting`, `SW.AutoFire`, `SW.ManualFire` and the `SW.AITargeting` modes by mapping each mode onto the existing `Pick_*` functions and adding the missing ones (`Self`, `Base`, `EnemyBase`, `Attack`, `LowPower`, `LowPowerAttack`, `LightningRandom`, `Stealth`, `NoTarget`) in `HouseClass::AI_Super_Weapons`. Add Phobos `AISuperWeaponDelay` with a per-house `CDTimerClass<FrameTimerClass>` and a call from `HouseClass::AI`; skip the call in `Expert_AI` when the key is set.
7. Chaining and Phobos delivery: `SW.Next`, `SW.Link`, rolls and weights, `SW.Aux/NegTechnos` and `SW.TechLevel` (in the availability function), `LimboDelivery/LimboKill` (new per-building `LimboID`, a per-house list, a count table by type for the aux checks), `Convert.*` (follow the other spec), `SW.MaxCount`.
8. Per-`Type=` values: capture `Lightning.*` and `Dominator.*` when `LightningStormClass::Start` and `PsychicDominatorClass::Start` run; `Chronosphere.*`, `Protect.*`, `Mutate.*`, `Nuke.*` (payload weapon per weapon), `ParaDrop.*` resolution, `SpyPlane.*`, `DropPod.*`; `Light.*` through the existing lighting code in `lstorm.cpp`/`psydom.cpp`.
9. New Types: `UnitDelivery`, `SonarPulse`, `HunterSeeker.*`, the EMPulse extension (replace the single `EMPDest` with a per-cannon target, add cannon list, count, linked, pulse ball, delay and the Phobos weapon index and suspension), `Battery` (hooks in the house power calculation, `Update_Present_Super_Weapons`).

## Saved state

- `SuperWeaponTypeClass`: every new parsed field goes into `Serialize` (the existing type already saves all its fields); raise `SaveVersionInfo::REVISION`. Pointers to types (warheads, weapons, animations, other weapons, building and techno types) must be swizzled like `Weapon` and `AuxBuilding`. Fields that change play belong in `Compute_CRC` so a rules mismatch shows up in the sync check; presentation fields do not.
- `SuperClass` (`Serialize`, `Compute_CRC`): shot count; `ever charged` flag for `SW.InitialReady` and the animation rule; virtual-charge state; drain payment timer for `Money.DrainDelay`. Count and flags go in the CRC; the timers match the existing `Control` handling.
- Pending effects for `SW.Deferment` and `Type=UnitDelivery`: house, weapon, cell, frames left. Global or per house; saved and in the CRC.
- Singleton storm and dominator: the pointer to the weapon type (or the copied values) that started them.
- `BuildingClass`: `LimboID` (int, default -1) and an `is limbo-delivered` marker; `CurrentEMPulseSW` pointer for tagged cannons.
- `HouseClass`: the list of limbo-delivered buildings (rebuilt on load from the marker is acceptable), per-type limbo counts used by the aux checks, `AISuperWeaponDelayTimer`, the map of suspended EMPulse weapons.
- Random-roll results are not stored; the synchronised generator state already saved by the scenario decides them.
- Nothing from the cursor, EVA, message and caption keys is saved.
- Network: a new event is not needed; `SPECIAL_PLACE` carries index and cell. With `SW.UseAITargeting` the click must either send the chosen cell or send the click and let every client choose with the synchronised generator (owner decision 23).

## Open questions

Needs a comparison test with real Ares (documentation leaves it open or two pages disagree):

1. How Ares resolves several weapons that share an `Action=`. The engine fires the earliest declared; the Ares docs are silent. Proposed: fire the armed weapon.
2. Whether building upgrades count in `AuxBuilding`, `SW.AuxBuildings` and `SW.NegBuildings`. The Ares page says they do not; Phobos code counts them.
3. `SW.Range` for LightningStorm: radius (general page) or diameter (Lightning page).
4. Meaning of `empty` in `SW.AffectsTarget`/`SW.RequiresTarget`, and of `none` there.
5. Distance metric for `SW.RangeMinimum/Maximum`, `DesignatorRange` and `InhibitorRange` (squared straight-line cell distance from the centre cell is Phobos').
6. `SW.RangeMinimum` default for EMPulse: 0.0 on its page, -1.0 on the range page.
7. Order of checks at fire time (range, designators, inhibitors, funds, concurrent storm) and which message wins when several fail.
8. `SW.VirtualCharge`: timer behaviour while the weapon is suspended or unpowered; what `SW.InitialReady` does on a second grant.
9. Whether `SW.Shots` survives a weapon being removed and granted again, and whether a one-time grant uses a shot.
10. `Money.Amount` timing: charged at click, at launch or at the end of the deferment; refund if the shot fails.
11. Where several Lightning Storm or Dominator weapons run at once (the `_Inactive` constraints suggest one at a time globally).
12. Cursor shown over shroud, fog and off-map cells; whether the minimap accepts the new cursors.
13. `SW.AnimationVisibility` default, and whether `SW.Animation` plays for the firer's allies without LOS.

Phobos oddities and bugs (read from its code):

14. `HandleEMPulseLaunch` records an un-suspend entry with the firing weapon's own index in the branch that finds an existing entry, and with the suspended weapon's index in the other branch; the first looks wrong, so a suspended weapon may stay suspended.
15. `HasInhibitor` and `HasDesignator` return from inside the loop over the listed types, so only the first listed type with a non-zero range is checked (same for `SW.AnyInhibitor`/`AnyDesignator`, first type in the type array).
16. `SW.Next` has no cycle guard: a weapon that lists itself, or two that list each other, recurses until the stack overflows. `SW.Next.RealLaunch=no` restores the timer values after the shot but leaves the ready flag cleared, and the shot counter of the next weapon still rises.
17. Whether an EMP-disabled or temporal-frozen building counts as a launch site (the source read shows only the powered-on test).
18. Docs and code disagree: `SW.Next.RealLaunch` ("must own all" vs per-entry skip), `Detonate.*` ("respects range" vs only picks the credited building). Behaviour for an observer as local player (an observer counts as allied with every house in the game's alliance test, so would get the allies line) and for a weapon with no providing building is undocumented.
19. `SW.Link` code treats a house as "observer" with an expression that is true for almost every house; the intent looks like "skip observers".

Owner decisions:

20. Whether to match the Phobos bugs in 14 to 16 (default: no; check every listed type, guard `SW.Next` depth, restore the ready flag).
21. Whether OpenYR keeps reading `PreClick`, `PostClick` and `PreDependent` from the INI (the engine does; Ares ignores them and hard-codes the pairing) and how `SW.PostDependent` overrides them.
22. Whether `Type=` names the engine does not know (e.g. `Battery` before it exists) leave the previous type, as the code does now, or log and disable the weapon.
23. Choosing the `SW.UseAITargeting` and `SW.AutoFire` target: at click time in the event, or deterministically at execution.
24. Numbering of the new Types in the enum. Saves record the integer, so new names go at the end of the list.
25. Whether `[General] AISuperWeaponDelay` applies to the Phobos "neutral" test as written (`IsNeutral`); the engine's nearest flag is `Class->IsMultiplayPassive`.

## Test plan

Write each test as a rules overlay in a small test mod and run it in a skirmish. Use `RechargeTime=0.1` (6 seconds) and set start credits to 10000. Add test weapons to `[SuperWeaponTypes]` with a high number, for example `90=TestIC`, and give them to `[GAPOWR]` (Allied Power Plant) with `SuperWeapon=` or `SuperWeapon2=`. Copy `SidebarImage` from an existing weapon. Run the stock-versus-feature check first, with the keys removed, and record the real Ares result before treating an Ares expectation as settled.

1. **Availability lists (Ares).** `[TestIC]` `Type=IronCurtain`, `Action=IronCurtain`, `SW.AuxBuildings=GAPILL`, `SW.NegBuildings=GAWEAP`. As Americans build `GAPOWR`: no cameo and no detection voice (stock: cameo at once). Build `GAPILL`: the cameo appears and charges. Build `GAWEAP`: the cameo disappears. Sell `GAWEAP`: it returns.
2. **Countries and controller (Ares).** Same weapon with `SW.RequiredHouses=Russians`: Americans get no cameo. Change to `SW.RequiredHouses=Americans`: cameo. Add `SW.ForbiddenHouses=Americans`: no cameo. Set `SW.AllowPlayer=no` with no house list: a human gets no cameo and an AI house that builds `GAPOWR` does.
3. **Shot limit (Ares).** `SW.Shots=2`. Fire twice; the cameo disappears after the second shot while `GAPOWR` stands. Stock: unlimited.
4. **Tech level and techno aux (Phobos).** `SW.TechLevel=5`, skirmish tech level 4: no cameo; tech level 5 or more: cameo. Then `SW.AuxTechnos=E1` with no GI: none; train one GI: cameo; load the GI into any transport (a passenger is in limbo): the cameo stays. Replace the list with `SW.NegTechnos=E1` and the cameo is removed while a GI exists.
5. **Generic warhead (Ares).** `[TestBlast]` `Type=GenericWarhead`, `Action=LightningStorm`, `SW.Warhead=TestWH`, `SW.Damage=400`; `[TestWH]` a copy of `[HE]` with `CellSpread=2.5`. Fire at a group of your own infantry and a tank (the Ares warhead hits every house): infantry within about 2 cells die and the tank takes damage. Set `CellSpread=0`: record whether the infantry standing in the target cell are hit (the documentation predicts they are missed).
6. **Money (Ares).** `Money.Amount=-5000`, credits 10000. First and second shots succeed (credits 5000, then 0). The third is refused: no effect, charge kept, `EVA_InsufficientFunds` plays and the credits stay 0. With `Money.Amount=2000` each shot adds 2000.
7. **Range band (Ares).** `SW.RangeMinimum=3`, `SW.RangeMaximum=8`, one providing building. Aim 2 cells from the building's centre: `NoCursor` (Disallowed); 5 cells: normal cursor; 10 cells: `NoCursor`. Aim only well inside each band; the metric is open question 5.
8. **Designator (Ares).** `SW.Designators=E1` and `[E1]` `DesignatorRange=4`. With a GI placed in the field, the cursor is normal within 4 cells of the GI and `NoCursor` beyond. After the GI dies it is `NoCursor` everywhere.
9. **Two weapons on one action.** `[TestIC]` and `[TestIC2]` both `Action=IronCurtain`, given as `SuperWeapon=` and `SuperWeapon2=` of `GAPOWR`, with `Message.Launch=MSG:CannotFire` only on `TestIC2`. Click the second cameo and fire. The engine today fires `TestIC` (no message). Record which weapon real Ares fires.
10. **Presentation (Ares).** `Text.Ready=TXT_RELEASE`, `EVA.Ready=none`, `EVA.Activated=EVA_IronCurtainActivated`, `Message.Launch=MSG:CannotFire`. When charged the cameo reads "Release" and no ready voice plays. Firing prints the message to every player and plays the activation voice once.
11. **Recipient lines (Phobos).** Needs two or more human clients. Four players, A and B allied, C and D allied against them, A fires. `Message.Activated.Owner=MSG:CannotFire` shows only on A's screen; `.Allies` only on B's; `.Enemies` on C's and D's. With the same three `EVA.Activated.*` entries each client hears only its own line. Repeat with an observer: record which line the observer gets.
12. **Chain (Phobos).** `[TestA]` `SW.Next=TestB`; both `Type=GenericWarhead` with different warheads or animations. Fire A with B charged: both effects land on the same cell, B's cameo restarts. Fire A with B still charging: only A fires. Set `SW.Next.RealLaunch=no`: B fires with its charge unchanged. Edge: `SW.Next=TestA` on a copy of the weapon; record the result (the Phobos code recurses without a limit).
13. **Link (Phobos).** `[TestA]` `SW.Link=TestB`, `SW.Link.Grant=yes`, `Message.LinkedSWAcquired=MSG:CannotFire`. Fire A: B's cameo appears (charging) and the message shows. Fire B once: the cameo disappears and does not return until A fires again. Add `SW.Link.Ready=yes`: B arrives charged. Make B a normal weapon already held and charging: with `SW.Link.Reset=yes` its charge restarts when A fires; with neither key set nothing changes and no message shows.
14. **Limbo delivery (Phobos).** `LimboDelivery.Types=GAPOWR`, `LimboDelivery.IDs=7`; second weapon `LimboKill.IDs=7`. Fire the first: the power output rises by the building's `Power=` value, no building appears on the map, and nothing can select or attack it. Fire the second: the output falls back, with no explosion or wreck. With `LimboKill.Counts=1` and two deliveries, one fire removes one.
15. **Detonation (Phobos).** `Detonate.Warhead=TestWH`, `Detonate.Damage=400` on a weapon that provides an ordinary effect: the warhead lands on the target cell as well. Add `Detonate.AtFirer=yes`: it lands on the providing building instead. Use `SW.AlwaysGranted=yes` with `AtFirer`: no detonation (no building).
16. **Sidebar tab (Phobos).** `TabIndex=3`: the cameo shows on the vehicle tab; `TabIndex=7`: on tab 1 with a log line.
17. **AI delay (Phobos).** Watch a computer house with a lightning storm and `ShowTimer=yes`. Stock: it fires at most 112 frames (7.5 seconds) after the timer reads `00:00`. With `[General] AISuperWeaponDelay=0` it fires within about 2 frames. With `300` it fires up to 300 frames (20 seconds) after, and over several launches some waits exceed 112 frames.
18. **EMPulse index (Phobos, needs a mod cannon).** A building with `EMPulseCannon=yes`, a primary and a secondary weapon with different warheads, and `Type=EMPulse` weapon with `EMPulse.WeaponIndex=1`: the secondary weapon's projectile flies. With the key at 0 or unset the primary flies. Two EMPulse weapons on the same cannon with indices 0 and 1, fired in turn before the cannon finishes: the later index wins. With `EMPulse.SuspendOthers=yes` the other weapon's cameo is on hold until the first cannon starts firing.
