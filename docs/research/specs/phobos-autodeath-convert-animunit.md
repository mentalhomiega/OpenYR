# Kill Object Automatically, type conversion and Anim-to-Unit (Phobos)

Source: Phobos `docs/New-or-Enhanced-Logics.md`, sections "Kill Object Automatically", "Automatic conversion based on ammo", "Automatic conversion based on health", "Convert TechnoType on owner house change", "Allow infantry to perform type conversion when deploying and undeploying", "Convert TechnoType" (superweapon), "Convert TechnoType on impact" (warhead), "Reset MindControl after transformation" and "Anim-to-Unit"; `docs/Fixed-or-Improved-Logics.md` section "Destroy animations". Code: `src/Ext/Techno/Body.Update.cpp` (`CheckDeathConditions`, `KillSelf`), `src/Ext/Techno/Body.cpp` (`ConvertToType`), `src/Ext/Foot/Body.cpp` (`UpdateTypeData`, `AmmoAutoConvertActions`, `HealthAutoConvertActions`), `src/Ext/House/Hooks.cpp`, `src/Ext/Scenario/Body.cpp`, `src/Ext/Infantry/Hooks.cpp`, `src/New/Type/Affiliated/TypeConvertGroup.cpp`, `src/Ext/WarheadType/Detonate.cpp`, `src/Ext/SWType/FireSuperWeapon.cpp`, `src/Ext/Anim/Hooks.AnimCreateUnit.cpp`, `src/Ext/Anim/Body.cpp`, `src/Ext/AnimType/Body.cpp`, `src/Ext/TechnoType/Body.cpp` (`CreateUnit`), `src/New/Type/Affiliated/CreateUnitTypeClass.cpp`.

Three features share one primitive, the in-place type change of a foot object, and the spec treats it once under "Type conversion". Phobos hands the change to Ares 3.0 or later when Ares is loaded and runs its own fallback otherwise; this spec describes the Phobos fallback and the Phobos code that runs after either path. What the Ares routine itself does is not visible from Phobos and is listed under "Open questions".

## Keys

### rules.ini `[CombatDamage]` and `[General]`

| Key | Section | Type | Default |
|---|---|---|---|
| `AutoDeath.AllowLimboed` | `[CombatDamage]` | boolean | `true` |
| `AutoDeath.OnOwnerChange.IgnoreRevertOnExit` | `[CombatDamage]` | boolean | `false` |
| `AutoDeath.TechnosDontExist.AllowLimboed` | `[CombatDamage]` | boolean | `false` |
| `AutoDeath.TechnosExist.AllowLimboed` | `[CombatDamage]` | boolean | `false` |
| `Convert.ResetMindControl` | `[General]` | boolean | `false` |

### rules.ini TechnoType: AutoDeath

Any key below has an effect only when `AutoDeath.Behavior` is set.

| Key | Type | Default |
|---|---|---|
| `AutoDeath.Behavior` | `kill`, `vanish` or `sell` | unset (feature off) |
| `AutoDeath.AllowLimboed` | boolean | `[CombatDamage]` value |
| `AutoDeath.VanishAnimation` | list of AnimationTypes | empty |
| `AutoDeath.OnAmmoDepletion` | boolean | `false` |
| `AutoDeath.OnOwnerChange` | boolean | `false` |
| `AutoDeath.OnOwnerChange.IgnoreRevertOnExit` | boolean | `[CombatDamage]` value |
| `AutoDeath.OnOwnerChange.HumanToComputer` | boolean | value of `AutoDeath.OnOwnerChange` |
| `AutoDeath.OnOwnerChange.ComputerToHuman` | boolean | value of `AutoDeath.OnOwnerChange` |
| `AutoDeath.AfterDelay` | integer frames; only values above 0 act | `0` |
| `AutoDeath.TechnosDontExist` | list of TechnoTypes | empty |
| `AutoDeath.TechnosDontExist.Any` | boolean | `false` |
| `AutoDeath.TechnosDontExist.AllowLimboed` | boolean | `[CombatDamage]` value |
| `AutoDeath.TechnosDontExist.Houses` | affected-house list (`owner`/`self`, `allies`/`ally`, `enemies`/`enemy`, `neutral`, `team`, `others`, `all`, `none`; comma-separated) | `owner` |
| `AutoDeath.TechnosExist` | list of TechnoTypes | empty |
| `AutoDeath.TechnosExist.Any` | boolean | `true` |
| `AutoDeath.TechnosExist.AllowLimboed` | boolean | `[CombatDamage]` value |
| `AutoDeath.TechnosExist.Houses` | affected-house list | `owner` |
| `AutoDeath.PlayerPowerState` | `none`, `low`/`consumer`, `full`/`normal` | `none` |
| `AutoDeath.PlayerMoney.Max` | integer credits | `-1` (unused) |
| `AutoDeath.PlayerMoney.Min` | integer credits | `-1` (unused) |

The Phobos documentation's INI block names the last two keys `AutoDeath.PlayerMoneyLessThan` and `AutoDeath.PlayerMoneyMoreThan`. The code and the documentation's prose read `.Max` and `.Min`, so `.Max` and `.Min` are the keys.

### rules.ini TechnoType: conversion triggers

| Key | Type | Default |
|---|---|---|
| `Ammo.AutoConvertMinimumAmount` | integer | `-1` (off) |
| `Ammo.AutoConvertMaximumAmount` | integer | `-1` (off) |
| `Ammo.AutoConvertType` | TechnoType | none |
| `Convert.Health.AbovePercent` | float, `0.5` or `50%` | `-1.0` (off) |
| `Convert.Health.BelowPercent` | float | `-1.0` (off) |
| `Convert.Health` | TechnoType | none |
| `Convert.HumanToComputer` | TechnoType | none |
| `Convert.ComputerToHuman` | TechnoType | none |
| `Convert.Deploy` | TechnoType (acts on infantry only, see Behaviour) | none |
| `Convert.Undeploy` | TechnoType (acts on infantry only) | none |
| `Convert.ResetMindControl` | boolean | `[General]` value |

`Convert.Deploy` is also an Ares key for `IsSimpleDeployer` vehicles. Phobos reads one value for both uses.

### rules.ini WarheadType and SuperWeaponType

| Key | Type | Warhead default | Superweapon default |
|---|---|---|---|
| `ConvertN.From` (N = 0, 1, 2, ...) | list of TechnoTypes; empty means any | empty | empty |
| `ConvertN.To` | TechnoType | none; a pair without it ends the numbered list | same |
| `ConvertN.AffectsHouse` | affected-house list | `all` | `owner` |
| `Convert.From`, `Convert.To`, `Convert.AffectsHouse` | the same, replacing pair 0 | as above | as above |

The older spellings `ConvertN.AffectedHouses` and `Convert.AffectedHouses` are still read and log a warning; the new spelling wins if both are present.

### art.ini AnimationType

| Key | Type | Default |
|---|---|---|
| `CreateUnit` | TechnoType (not a BuildingType) | none (feature off) |
| `CreateUnit.Owner` | `default`, `invoker`, `killer`, `victim`, `civilian`, `special`, `neutral`, `random` | `victim` |
| `CreateUnit.RequireOwner` | boolean | `false` |
| `CreateUnit.RemapAnim` | boolean | `false` |
| `CreateUnit.Mission` | mission | `Guard` |
| `CreateUnit.AIMission` | mission | unset (uses `CreateUnit.Mission`) |
| `CreateUnit.Facing` | integer 0 to 255 (0 north, 64 east) | `0` |
| `CreateUnit.RandomFacing` | boolean | `true` |
| `CreateUnit.InheritFacings` | boolean | `false` |
| `CreateUnit.InheritTurretFacings` | boolean | `false` |
| `CreateUnit.AlwaysSpawnOnGround` | boolean | `false` |
| `CreateUnit.SpawnParachutedInAir` | boolean | `false` |
| `CreateUnit.ConsiderPathfinding` | boolean | `false` |
| `CreateUnit.SpawnAnim` | list of AnimationTypes | empty |
| `CreateUnit.SpawnHeight` | integer leptons | `-1` |

The documentation says `CreateUnit.SpawnHeight` acts when "positive"; the code acts on 0 and above. The documentation lists the owner kinds without `default`, which the parser accepts.

## Behaviour

### AutoDeath: when the check runs and in what order

- The check runs at the start of `TechnoClass::AI` for every object, after the shield, attached-effect, passenger-deletion and similar updates and before the interceptor logic. It is not gated by EMP, deactivation, cloak or Iron Curtain. Ammo and timer conditions therefore keep counting while the object is disabled.
- Objects that are not in the logic list (passengers, bunkered, absorbed or garrisoned objects, and objects created inside a transport) are checked once per frame at the start of the logic update. Phobos keeps a list of every AutoDeath object, registered at first placement on the map, at LimboDelivery, and for team, initial-payload and occupant objects, and runs the check on those that are alive and outside the logic list.
- Order inside one check, stopping at the first kill:
  1. No `AutoDeath.Behavior`: nothing happens.
  2. Object in limbo and `AutoDeath.AllowLimboed` false: nothing happens.
  3. Pending kill flag set by an earlier owner change or by a sibling object (see below): kill.
  4. `AutoDeath.OnAmmoDepletion`: the type's `Ammo` is above 0 and the object's current ammo is 0 or less: kill.
  5. `AutoDeath.AfterDelay` above 0: the timer starts on the first check that reaches this step and the kill happens when it completes. The timer is a frame timer. An object delivered by LimboDelivery starts its timer at delivery.
  6. If the owner-condition skip mark is set, clear it and stop here.
  7. `AutoDeath.PlayerPowerState`: `low` kills while the owner is in low power, `full` kills while it is not.
  8. `AutoDeath.PlayerMoney.Max` / `.Min` (credits plus stored tiberium value): with only `.Max` set, kill while money is at or below it; with only `.Min`, kill while money is at or above it; with both, kill while money is inside the range, ends included. The type loader logs a warning if `.Min` is above `.Max`, since the object then never dies from money.
  9. `AutoDeath.TechnosDontExist`, then 10. `AutoDeath.TechnosExist`.
- Exist lists, in terms of the observable rule:
  - `TechnosExist` kills when at least one listed type exists (`.Any=true`, the default) or when every listed type exists (`.Any=false`).
  - `TechnosDontExist` kills when every listed type is missing (`.Any=false`, the default) or when at least one listed type is missing (`.Any=true`).
  - "Exists" means the checked houses own at least one object of the type that is on the map; with `.AllowLimboed` true it also counts objects in transports and LimboDelivery buildings. `.Houses` selects the houses: `owner` checks only the object's owner, any other value checks every house the filter accepts relative to the owner.

### AutoDeath: per-frame cost

- An object without `AutoDeath.Behavior` pays one test per frame.
- An object with it pays the ammo and timer tests every frame.
- Owner conditions (steps 7 to 10) are evaluated once per frame for all objects of the same type and owner. The first member of the group to run its AI evaluates them. If nothing triggers, it sets a skip mark on every same-type, same-owner object (found by scanning the type's instance list, which spans all owners), and each of those clears its mark and skips steps 7 to 10 when its own AI runs. If a condition triggers, it kills itself and sets the pending kill flag on every same-type, same-owner object, so the whole group dies in the same frame.
- A group therefore costs one scan of the type's instance list per frame, plus for each exist list one count lookup per listed type per checked house. No random numbers are used, so the result depends only on frame state and object order, which is the same on all clients.

### AutoDeath: the three behaviours

- `kill`: the object takes damage equal to its current health from the `C4Warhead`, with no attacker and with defenses ignored. It dies through the normal death path (death weapon, explosion and destroy animations, wreck, loss accounting). With Ares loaded, vehicles and aircraft first spawn survivors with no killer. Without Ares, passengers are not released.
- `vanish`: a random `AutoDeath.VanishAnimation` plays at the object's position (owner and invoker are the object itself), a bunkered building is unloaded, passengers are killed with the object as killer, and the object is stunned, put into limbo, registered as a kill by its own owner and removed. No death weapon, wreck or death animation runs.
- `sell`: a building that has a build-up is put on the selling mission unless it is already selling, and the sale is an ordinary sale. Any other object is killed as under `kill` (Phobos logs a developer warning). LimboDelivery buildings are deleted outright under every behaviour.
- An object in limbo when the check kills it is always removed silently, whatever the behaviour: parasites are expelled, its tracking is removed, it is registered as a kill by its own owner, and the transport's gunner is reassigned.

### AutoDeath: owner change

- The owner-change flag is set inside the game's owner-change routine, so it covers every path through it, including mind control and its release, capture and the change-owner warhead. The paths were not enumerated. The flag is set before the owner pointer changes, and the kill happens on the object's next AI call under the new owner.
- With both `.HumanToComputer` and `.ComputerToHuman` true (the default when `AutoDeath.OnOwnerChange=yes`), any owner change kills. With one true, only a change of control type in that direction kills: from a human-controlled house to a computer-controlled house for `.HumanToComputer`, the reverse for `.ComputerToHuman`. With both false, nothing happens.
- `.IgnoreRevertOnExit` true clears the pending flag when the change is the owner reverting as a passenger leaves a `Passengers.SyncOwner.RevertOnExit` transport.
- A pending flag is also cleared when the object is inside a transport, `AutoDeath.AllowLimboed` is false and the change is not a revert on exit.

### AutoDeath: interactions

- A conversion to a type without `AutoDeath.AfterDelay` stops a running timer. A conversion to a type that has one leaves a running timer at its remaining time. A conversion to a type without `AutoDeath.Behavior` removes the object from the limbo-polling list; a conversion to a type that gains one does not add it to that list.
- The AutoDeath check runs before the ammo and health conversion checks in the same frame. An object with `AutoDeath.OnAmmoDepletion` and `Ammo.AutoConvertMaximumAmount=0` dies and never converts.
- Phobos also shows the remaining AutoDeath time or ammo in its info display; that display is a separate feature.

### Type conversion: the operation

- Entry point: convert a foot object (infantry, vehicle or aircraft) to a TechnoType of the same kind. Buildings are never converted. A target that equals the current type, or is of another kind, is refused with a log line and nothing changes.
- Phobos fallback, in order:
  1. A temporal weapon the object holds on a target releases the target.
  2. The old type is removed from the owner's "present on the map" counts (skipped for an object in limbo) and from its "owned" counts; the type pointer changes; the new type is added to both; the owner's tech tree is flagged for recheck.
  3. Health keeps its ratio: new health is old health divided by the old type's strength, times the new strength.
  4. Ammo becomes the smaller of the current ammo and the new type's `Ammo`.
  5. The turning rate becomes the new type's `ROT` (the secondary facing for aircraft, the primary facing otherwise).
  6. If the locomotor class differs, the old locomotor is dropped and a new one of the new type is created and linked. Phobos marks this step as untested.
  7. A conversion from a non-jumpjet locomotor to a jumpjet type that has `BalloonHover` and `DeployToLand` sends the object to its own position, which makes it hover.
- Phobos then runs its type-data update after either path:
  - Per-type bookkeeping: the object moves between the types' instance lists; type-defined attached effects, shield type (honouring `AllowTransfer.Convert`) and tint are refreshed; laser trails of the old type are replaced by those of the new type while trails added by effects stay.
  - Timers and lists: see "AutoDeath: interactions". The passenger-deletion timer stops if the new type has no deletion rate; harvester and transport-reload lists follow the new type.
  - Slaves and spawns: a missing slave or spawn manager is created and one the new type lacks is removed. When the counts differ, nodes are added as dead (they respawn after the regeneration time) or the excess is removed: slaves in limbo are erased and others killed; spawned aircraft that are idle, reloading or taking off are erased, spawned missiles are exploded, and other spawns crash.
  - Weapon-driven managers, decided from every weapon of the new type: a mind-control warhead creates a capture manager if there is none (its limit is the largest mind-control damage among the weapons); a temporal warhead creates the temporal weapon and none removes it, releasing a held target; an airstrike warhead with `AirstrikeTeam` above 0 creates or updates the airstrike; a locomotor-inflicting weapon is required to keep a locomotor target, otherwise it is released; a parasite warhead creates the parasite and none removes it, expelling the victim.
  - `Convert.ResetMindControl` on the old type, when true: with a mind-control weapon on the new type the control limit becomes the new one and units beyond it are released unless `InfiniteMindControl` is set; without one, all controlled units are released and the manager is deleted. When false, controlled units stay, and an existing manager keeps its old limit.
  - Sound and sensors: a moving object restarts its move sound if `MoveSound` differs; bomb-sight detector membership follows the type.
  - Infantry that leave a `Deployer` type while in a deploy sequence return to ready (or fire-up when firing).
  - A foot object leaving the Teleport locomotor while warping out keeps a pending warp-in delay.
  - Passengers: whenever the object carries passengers, they are re-synchronised with the new type's `OpenTopped`. An open-topped type adds them to the logic list; otherwise they are removed from it and lose their target and destination. The gunner is reassigned.
  - Disguise is cleared when the new type cannot disguise or when a permanent disguise has been lost.
  - Non-aircraft, when the new type's locomotor is not Fly or Hover: an object that now uses a jumpjet locomotor gets the new type's jumpjet parameters (speed, climb, accel, turn rate, height, wobbles, crash, deviation) and, if it is in the air, hovers or is ordered to its own position. An object in the air under any other locomotor starts to fall; it crashes on landing if its cell is impassable for the new type, and infantry play the paradrop sequence. Vehicles whose `Turret` setting differs set the turret to face the body. The barrel angle is reset from `FireAngle` and the recoil data from the new type.
  - Aircraft: strafing state is reset when the new weapon does not strafe; the dock target is cleared for types that are not `AirportBound`.
  - An object with `AutoTargetOwnPosition` clears its target when the new type lacks it. Ares alpha images are dropped.
- State the Phobos fallback and update code leave alone: veterancy and experience, selection and control-group membership, team membership, mission, target and destination (except where listed), cargo, body facing and turret facing (except the turret case above), cloak state, Iron Curtain and non-transferred shield status, owner, position, and the pending AutoDeath flag. Armor and every other value read from the type follow the new type.
- Anything that moves the object between types in one frame (a warhead pass, a superweapon launch) can chain: a later pair or a later trigger converts the new type again.

### Conversion triggers

- Ammo (`Ammo.AutoConvertType` with a minimum and/or maximum): checked every frame for every foot object after its Techno AI, whether or not the ammo changed. Applies only when the current type has `Ammo` above 0. Converts when ammo is at or above the minimum (if the minimum is 0 or more) and at or below the maximum (if the maximum is 0 or more). With the type unset, or both limits negative, it never converts. A log line warns when the minimum is above the maximum.
- Health (`Convert.Health` with `.AbovePercent` and/or `.BelowPercent`): checked every frame in the same place. The ratio is current health over the current type's strength. Converts when the ratio is strictly above `.AbovePercent` (a ratio of exactly 0 passes if it is at or above it) and at or below `.BelowPercent`. The documentation says a negative value disables its own check; open question 1 describes what the code does with `.AbovePercent` alone.
- Both checks run on the new type from the next frame. Two types whose conditions overlap convert back and forth every frame.
- Owner change (`Convert.HumanToComputer`, `Convert.ComputerToHuman`): runs where the owner changes, before the object moves between house lists, only for foot objects and only when the human-or-computer control of the old and new owner differ. Human to computer uses `Convert.HumanToComputer`; computer to human uses `Convert.ComputerToHuman`. The owner's tech tree is rechecked for both houses.
- Infantry deploy and undeploy (`Convert.Deploy`, `Convert.Undeploy`): the conversion happens once when the Deploy sequence finishes and once when the Undeploy sequence finishes. The target comes from the type the infantry has at that moment. A per-object flag prevents a second conversion until the infantry leaves the Deploy and Undeploy sequences.
- Warhead (`ConvertN.*`): applies at detonation to each foot object that passes the warhead's usual target tests, which are owner/allies/enemies flags, health and veterancy thresholds, and verses when `EffectsRequireVerses` is set. Objects in limbo, dead objects and objects being warped out or sinking are skipped. A warhead with `CellSpread` above 0 applies to every object in the spread; with `CellSpread=0` it applies only to the bullet's own target, and only if the bullet is within 64 leptons of it. Per target, the pairs are tried in index order and the first pair that is valid (has `To`, house filter passes, `From` empty or containing the current type) converts the object and ends the search. The house filter is applied between the firing house and the target's owner.
- Superweapon (`ConvertN.*`): applied once at launch, independent of the target cell and of the superweapon's `Type=`. For a pair with a `From` list, every object of each listed type is converted, including passengers and units in bunkers or absorbed by buildings, if the owner passes `AffectsHouse` relative to the launching house. For a pair with an empty `From`, every foot object is run through all pairs in order, and the loop then repeats for the next pair, so an object can convert more than once (open question 3).
- Numbered pairs are read from index 0 and the list ends at the first index without a `To`. A non-numbered `Convert.*` replaces pair 0.

### Anim-to-Unit

- When an animation with `CreateUnit` reaches the point where stock YR runs `MakeInfantry` (the end of its last loop and chain), Phobos creates the unit and then still runs `MakeInfantry` if the type also sets it. While the animation plays, its cell is marked occupied so other units do not path into it; the mark is cleared just before the unit is created.
- Owner: the animation's owner is chosen when the animation is created, from `CreateUnit.Owner` and the parties known at that call site.
  - `victim`: the owner of the object that suffered the effect. `invoker` and `killer` both mean the house that caused it. `civilian`, `special` and `neutral` pick the first house of that kind. `random` picks any house, including civilian and special houses, with the synchronized random generator. `default` takes the call site's default.
  - Call sites and their parties: a vehicle's `DestroyAnim` (victim is the vehicle's owner at death, invoker is the attacker's owner or the damage source's house, and the default is none); a warhead's `AnimList`/`SplashList` (invoker is the firer's owner or the stored firing house, victim is the owner of the bullet's technoed target, and an animation left without an owner takes the invoker); the trigger action "Play Anim At" (invoker is the trigger's house, no victim); other animations created by Phobos code use the owning techno or house as invoker.
  - When the chosen kind has no matching party, a vehicle destroy animation stays ownerless; warhead animations and the other call sites give the animation to the invoker's house. A vehicle killed with no attacker and no source house, such as by AutoDeath `kill`, has no `killer` or `invoker`.
  - When the animation ends, a missing or defeated owner gives the unit to the first house of the Civilian side, or creates nothing when `CreateUnit.RequireOwner=yes`.
  - `CreateUnit.RemapAnim` tints the animation with the owner's color scheme when the owner is known and not defeated at creation time.
- Facing: with `CreateUnit.RandomFacing=yes` (the default) the facing is a random value 0 to 255 from the synchronized generator, drawn once for each unit created, and `CreateUnit.Facing` is ignored. With `no`, no random number is drawn. `CreateUnit.InheritFacings` replaces the facing with the destroyed vehicle's body facing and `CreateUnit.InheritTurretFacings` sets the turret facing from the destroyed vehicle's turret. Both apply only to animations that came from a vehicle's `DestroyAnim`, and the turret facing applies only to a created vehicle that has a turret.
- Position: the animation's location. Height is the animation's height unless `CreateUnit.AlwaysSpawnOnGround=yes` (ground level) or `CreateUnit.SpawnHeight` is 0 or above (that height above the floor of the cell, bridge height included). A position counts as in the air at a height of two cell levels or more.
  - `CreateUnit.ConsiderPathfinding=yes`: if the cell is not clear for the unit's speed type, the nearest clear cell replaces it.
  - With `ConsiderPathfinding=no`, or when the cell has no building, the unit is placed without placement checks, so it can share a cell with another unit. With the option on and a building in the cell, normal placement checks apply.
  - Aircraft use the Wheel speed type for these checks.
- In the air, unless `AlwaysSpawnOnGround=yes`: a non-aircraft object with `CreateUnit.SpawnParachutedInAir=yes` is created with a parachute; other ground-type objects start to fall; flying objects are set at the location and either idle (cell occupied, or airport-bound type) or ordered to move to the cell; jumpjets land, or ascend if `BalloonHover` is set.
- After creation: `CreateUnit.SpawnAnim` plays one random entry at the unit's location with the parent's invoker; the unit gets `CreateUnit.Mission` (or `CreateUnit.AIMission` for a computer-controlled owner, as decided by `IsControlledByHuman`); the owner's tech tree is rechecked unless the owner is a passive multiplayer house.
- Failure: if the unit cannot be created or placed, it is discarded and nothing retries. Stock `MakeInfantry` holds the last frame and retries.
- Created units cost nothing and pass no build-limit or factory check.
- Determinism: random facing (drawn at the end of the animation) and the `random` owner (drawn when the animation is created) use the synchronized generator at the same logic point on every client. Nothing else in `CreateUnit` is random.

## What stock YR does

- Nothing removes an object automatically. An object dies through damage, a sale, or a trigger or script action. Running out of ammo only stops firing.
- A foot object keeps its type for its whole life, with one fixed exception in this engine: the small Visceroid that merges into a large one. A harvester's unloading class replaces the type only while the object is drawn. Ares adds `Convert.Deploy`, `Convert.Water`, `Convert.Land`, `Convert.Script` and `Promote.*` conversions (`docs/research/ares-tags.md`); none of them triggers on ammo, health, owner control or warheads.
- An animation can create an infantry unit through `MakeInfantry`, which indexes `[General] AnimToInfantry`. The infantry belongs to the animation's owner, or to the Civilian side if there is none or it is defeated, faces south, and a computer-controlled owner sends it to hunt. If the cell is not free yet the animation holds its last frame and tries again. Vehicles and aircraft cannot be created this way. In this engine only the mutation animation is given an owner.

## Where it hooks in OpenYR

What exists now:

- No AutoDeath, `Convert.*`, `Ammo.AutoConvert*` or `CreateUnit` key is read (`manual/data/ini-keys.yaml` lists none). `manual/content` has no page for them.
- No general type conversion exists. The only in-place type write is the Visceroid merge in `UnitClass::Per_Cell_Process` (`code/unit.cpp`), which sets `Class` and strength directly. `UnitClass::Draw_It` swaps `Class` for the duration of a draw only.
- `AnimClass::Make_Infantry` (`code/anim.cpp`) is the stock `MakeInfantry` end, called from `AnimClass::AI`. `AnimClass` has `OwnerHouse` (a `HousesType`, saved and in the CRC) and no invoker. The only code that sets `OwnerHouse` is the infantry mutation animation in `code/infantry.cpp`. Warhead explosion animations (`BulletClass::Detonate` in `code/bullet.cpp`, `Combat_Anim` in `code/combat.cpp`) and the trigger "play animation" action (`TActionClass::TAction_PLAY_ANIM` in `code/taction.cpp`) create animations with no owner. `UnitClass::Explode` creates the `DestroyAnim` with no owner either.
- `TechnoClass::TClass` is a property that reads `Class` through `Class_Of`, so writing the per-class `Class` pointer changes the type for all type reads. Type-derived values that are copied into the object at creation are the locomotor (`Create_Locomotor` in the `UnitClass` and `InfantryClass` constructors), `PrimaryFacing`/`SecondaryFacing` `Set_ROT`, `Ammo` (`Initial_Ammo`), `IsCloakable`, `Strength`, and `Charge`. `TechnoClass::Unlimbo` creates the capture, temporal, spawn, slave and parasite managers from the primary weapon and the type's spawn and slave settings, and tests the primary weapon only; Phobos tests every weapon when it converts.
- House counts: `HouseClass::Tracking_Add`/`Tracking_Remove` (owned counts, `BQuantity` etc.), `Tracking_Active_Add`/`Tracking_Active_Remove` (present counts, `ABQuantity` etc.). `TechnoClass::Captured` and `TechnoClass::Set_Owning_House` (`code/techno.cpp`) update both around the owner change.
- Kill and removal: `TechnoClass::Take_Damage` with `forced=true` skips armor and the Iron Curtain test, and `Rule->C4Warhead` exists. `TechnoClass::Kill_Cargo`, `TechnoClass::Limbo`, `TechnoClass::Record_The_Kill` and `BuildingClass::Sell_Back` cover the rest of `KillSelf`.
- Per-frame hooks: `LogicClass::AI` (`code/logic.cpp`) runs each object's `AI()` by index; `TechnoClass::AI`, `FootClass::AI` and `InfantryClass::Doing_AI` are the equivalents of Phobos's hook points. `UnitClass::AI` and `BuildingClass::AI` return early when `Temporal_AI()` returns true, so an object in that state does not reach `TechnoClass::AI`; whether Phobos's hook is skipped in the same state was not checked.
- Rules reads: `RulesClass::Combat_Damage` and `RulesClass::General` (`code/rules.cpp`); type reads: `TechnoTypeClass::Read_INI` (`code/techtype.cpp`), `InfantryTypeClass::Read_INI`, `WarheadTypeClass::Read_INI`, `SuperWeaponTypeClass::Read_INI`, `AnimTypeClass::Read_INI`.

Build order, smallest first:

1. Parse all keys into the type classes and into `RulesClass`. The parse of the warhead and superweapon pairs can share one small `TypeConvertGroup` structure. No behaviour yet.
2. Write the conversion primitive as one `FootClass` routine, for example `FootClass::Convert_To_Type(TechnoTypeClass const *)`, covering the fallback steps above and the type-data update. Everything else calls it. Add `IsCloakable`, `Charge` and the other cached type values to the refresh list. Cover vehicle, infantry and aircraft; refuse buildings. Test it first with a debug command or a trigger action.
3. Add the warhead and superweapon triggers: the per-object loop in `Explosion_Damage` (`code/combat.cpp`) for warheads (honour the limbo, dead and sinking skips) and `SuperClass::Place` (`code/super.cpp`) for superweapons, iterating a copy of the `Technos` list.
4. Add the polled triggers after `BASECLASS::AI()` in `FootClass::AI`: ammo, then health. Add the owner-change conversion inside `TechnoClass::Captured` and `TechnoClass::Set_Owning_House` before `Tracking_Active_Remove`, using `HouseClass::Is_Human_Player()` as the human test. Add the infantry deploy and undeploy conversion where `InfantryClass::Doing_AI` sees a finished `DO_DEPLOY` or `DO_UNDEPLOY`, and then choose the next `Doing` from the new class, since `DoControls` and the stage count come from it.
5. Add AutoDeath: the per-object check at the start of `TechnoClass::AI`, an object list for limbo polling filled when an object first leaves limbo (`TechnoClass::Unlimbo`) or is attached to a transport at scenario start, and the three behaviours. Return from `TechnoClass::AI` after a kill. Iterate a copy of the polling list, because the kill removes entries. Rebuild the list in `Post_Load_Game` (`code/saveload.cpp`) instead of saving it. Use `Take_Damage(strength, 0, Rule->C4Warhead, NULL, true)` for `kill`.
6. Add anim ownership: give `AnimClass` an invoker and a "created from a vehicle" record, set the owner and invoker at the three call sites above with a shared house-kind helper, and make `UnitClass::Explode` pass the victim and killer into the destroy animation.
7. Add `CreateUnit` at the end of `AnimClass::AI` beside `Make_Infantry`, using `MapClass::Nearby_Location` and `CellClass::Is_Clear_To_Move` for pathfinding and the existing falling support (`IsFalling` in `code/object.h`) for the unplaced-in-air cases.

## Saved state

New per-object fields, all to be written by `Serialize` and added to `Compute_CRC` of the owning class, and the save revision raised in `code/savever.h` (`SaveVersionInfo::REVISION`; see `docs/SAVE-FORMAT.md`):

| Field | Class | Why |
|---|---|---|
| AutoDeath frame timer | `TechnoClass` | A running `AutoDeath.AfterDelay` must continue after a load and match on all clients. |
| AutoDeath flag (-1, 0, 1) | `TechnoClass` | A pending kill or skip mark can be outstanding at a frame boundary. |
| Deploy-converted and undeploy-converted flags | `InfantryClass` | Needed only if the conversion is guarded as Phobos guards it; an engine that converts at the end of the sequence and changes `Doing` at once does not need them. |
| Invoker object and invoker house | `AnimClass` | Used when the animation ends. The invoker pointer needs the same detach handling as other object pointers (`AnimClass::Detach`), since the invoker can die first. |
| Created-from-vehicle flag, body facing, has-turret flag, turret facing | `AnimClass` | Only the `CreateUnit.Inherit*` options read them. |

Type conversion needs no new field: the `Class` pointer is already saved per object (`UnitClass`, `InfantryClass`, `AircraftClass`) and `UnitClass::Compute_CRC` already includes the type's kind and ID, so a conversion that differs between clients is visible in the checksum. The house counters are already saved. Confirm the same CRC lines exist in `InfantryClass` and `AircraftClass`.

Not saved: the list of AutoDeath objects (rebuilt on load) and the skip marks' type lists (found by scanning). No new global or per-house field.

The `Convert.ResetMindControl`, `AutoDeath.*` and `CreateUnit.*` settings come from the INI files and are not saved.

## Open questions

1. `Convert.Health.AbovePercent` alone. With `.BelowPercent` unset (-1.0), the shared threshold test requires the health ratio to be at most -1.0, so a type with only `.AbovePercent` never converts. The documentation says a negative value disables only its own check. Either Phobos has a bug or the documentation is wrong. Owner decision: follow the documentation, and run test 10 on Phobos before deciding.
2. The documentation says ammo conversion happens "after the ammo update"; the code checks every frame. A unit whose ammo already satisfies the condition when it is created therefore converts at once, where "after the ammo update" would wait for the first ammo change.
3. Superweapon conversion with an empty `From` and more than one pair visits every foot object once per pair and tries all pairs each time, so one launch can convert an object through two pairs in sequence. Pairs with a `From` list can chain the same way (`A -> B` then `B -> C`). Owner decision: define a single pass with the first matching pair per object, or keep the chaining.
4. What Ares's own conversion routine does to health, ammo, locomotor, turret facing, attached effects, cached weapon state, veterancy and mind control is not visible. With Ares loaded, Phobos runs the Ares routine and none of the fallback steps listed above. The fallback is the only documented semantics; run test 14 with the real Ares and Phobos before treating it as the target.
5. `AutoDeath.PlayerPowerState`: the code's condition reads `full` OR (`low` AND not frame 0). The frame-0 guard therefore applies to `low` only, so a `full` check can kill on frame 0. This looks unintended. Owner decision: guard both or neither.
6. A conversion that gives an object `AutoDeath.Behavior` does not register it for limbo polling, so a converted passenger with `AutoDeath` is not checked while in a transport. A conversion between two types that both set `AutoDeath.AfterDelay` keeps the old countdown. Owner decision on both.
7. `kill` passes "defenses ignored" in Phobos; it is not tested whether an Iron Curtained or Phobos-shielded object dies. The engine's `forced` damage skips the Iron Curtain.
8. The kill-registration call Phobos makes for `vanish` and limbo removal, with the object's own owner as killer, may count in house loss and kill statistics. Which statistics change is not confirmed.
9. Warhead conversion skips objects in transports; superweapon conversion converts them. The documentation mentions it only for the superweapon.
10. Objects converted mid-sequence (deploying infantry, a vehicle in a firing animation): the code resets the infantry sequence only for the `Deployer` case, and the other cases were not examined.
11. A deployed infantry that moves away skips the Undeploy sequence in this engine (`InfantryClass::Do_Action` remaps its sequences), so `Convert.Undeploy` would not fire. Phobos behaviour for the same case is unconfirmed.
12. `CreateUnit.Owner=killer` and `invoker` are the same branch in the code. The documentation lists both as separate values.
13. A failed placement silently drops the unit in Phobos and `MakeInfantry` retries. Owner decision: match Phobos, or retry as `MakeInfantry` does.
14. Whether to refresh `IsCloakable` and `Charge` on conversion. Phobos has no equivalent field; this engine caches them.
15. Whether the capture, temporal and parasite managers should follow the primary weapon only (as at creation) or every weapon (as Phobos conversion does).
16. `Convert.Deploy` on vehicles (the Ares `IsSimpleDeployer` use of the same key) is outside this spec; it belongs to the Ares type-conversion work.
17. `Passengers.SyncOwner` with `RevertOnExit` changes a passenger's owner on entry and on exit. The order of that change against the passenger's transport link at exit decides whether `AutoDeath.AllowLimboed=no` keeps the passenger alive through the revert. This was not traced to the end and has no test here.

## Test plan

Frames are logic frames; at the base rate there are 15 per second. "Copy of X" means a new type section that repeats the stock section with `Image=X`, its own name and ID added to the matching `[...Types]` list. Run each test on a Phobos build first to record the result, then on OpenYR.

AutoDeath

1. Timer and behaviours. Copies of `E1` (GI): `E1K` with `AutoDeath.Behavior=kill`, `AutoDeath.AfterDelay=300`; `E1V` with `AutoDeath.Behavior=vanish`, `AutoDeath.AfterDelay=300`, `AutoDeath.VanishAnimation=` a short existing explosion animation. Build one of each next to a stock `E1`. Expect: about 300 frames after leaving the barracks `E1K` plays its normal death animation and scream; `E1V` disappears with the vanish animation, no death animation, scream or corpse; the stock `E1` stays alive.
2. Exist lists. `E1X` with `AutoDeath.Behavior=kill`, `AutoDeath.TechnosDontExist=GAPOWR,GAPILE`. Build a power plant, a barracks and then an `E1X`. Sell the plant: nothing happens, because the default `.Any=false` needs both types missing. Sell the barracks: the `E1X` dies on the next frame. Repeat with `AutoDeath.TechnosDontExist.Any=true`: selling either building kills it.
3. Group kill and limbo. Build five `E1X` under the conditions of test 2 and put one of them in a transport. Sell the buildings. Expect all five to die in the same frame, and the one in the transport to vanish without a death animation while the transport survives. Edge: set `AutoDeath.AllowLimboed=no` on `E1X` and repeat: the passenger survives while aboard.
4. Houses filter. `E1Y` with `AutoDeath.Behavior=kill`, `AutoDeath.TechnosExist=E2` and `AutoDeath.TechnosExist.Houses=enemies`. Expect it to die when an enemy Conscript exists and to survive Conscripts of its own side.
5. Money range. `E1M` with `AutoDeath.Behavior=kill`, `AutoDeath.PlayerMoney.Max=500`. Start with 2000 credits and spend until 500 or fewer remain: the `E1M` dies. Add `AutoDeath.PlayerMoney.Min=300` and spend down to 200: the `E1M` survives at 200 and dies at 400.
6. Power state. `E1P` with `AutoDeath.Behavior=kill`, `AutoDeath.PlayerPowerState=low`: it dies when the owner goes into low power (sell the only power plant). A copy with `full` dies on its first frames while power is sufficient (open question 5).
7. Sell and its fallback. A building copy with `AutoDeath.Behavior=sell` and `AutoDeath.PlayerPowerState=low` sells itself when low power starts and refunds credits. A tank copy with `AutoDeath.Behavior=sell` and `AutoDeath.AfterDelay=300` is destroyed with a normal death.
8. Ammo depletion. A copy of the Harrier (Allied aircraft with `Ammo=1`) with `AutoDeath.Behavior=kill` and `AutoDeath.OnAmmoDepletion=yes`. Send it on an attack run. Expect it to die right after its missile is fired instead of flying home to reload.
9. Owner change. `E1O` with `AutoDeath.Behavior=kill`, `AutoDeath.OnOwnerChange=yes`. In a skirmish against an AI, mind-control an AI-owned `E1O` with a Yuri Clone: it dies as it is captured. Set `AutoDeath.OnOwnerChange.ComputerToHuman=no` and repeat: it survives, and when the Yuri Clone dies and it reverts to the AI (a human-to-computer change) it dies.

Conversion

10. Health conversion. `MTNKH` (Grizzly copy) with `Convert.Health.BelowPercent=0.5`, `Convert.Health=MTNKL`, where `MTNKL` is a copy with `Strength` doubled and a different `Name`. Damage the tank to under 50%: it becomes `MTNKL` and its health bar keeps the same fraction. Edge: use only `Convert.Health.AbovePercent=0.5` on another copy, damage it below 50% and repair it above 50%, and record whether it converts (open question 1).
11. Ammo conversion and clamp. `MTNKA` (Grizzly copy) with `Ammo=3`, `Ammo.AutoConvertMaximumAmount=0`, `Ammo.AutoConvertType=MTNKB`; `MTNKB` with `Ammo=2`, `Ammo.AutoConvertMinimumAmount=1`, `Ammo.AutoConvertType=MTNKA`. Fire three shots: the tank becomes `MTNKB` on the frame after the third. Send it to a service depot: it converts back when its ammo reaches 1. Edge: convert a full `MTNKA` (ammo 3) with the warhead of test 13 into `MTNKC`, a copy with `Ammo=1`: its ammo becomes 1.
12. Preserved state. A transport vehicle copy `TRA` carrying two infantry, veteran, at 50% health, selected and assigned to control group 1, converted by the warhead of test 13 (`Convert.From=TRA`) to `TRB`, a copy with doubled `Strength`. Expect the veteran chevron, the selection and the group number to remain, the health bar to stay at 50%, the passengers to stay aboard, and the sidebar to re-evaluate the build options. Edge: convert a Yuri Clone copy that holds one mind-controlled unit into a copy with no mind-control weapon, once with `Convert.ResetMindControl=yes` (the victim is released) and once without it (the victim stays controlled).
13. Warhead and superweapon. A weapon whose warhead has `CellSpread=3`, `Convert.From=E1X,E2`, `Convert.To=E1Y`, `Convert.AffectsHouse=team`, fired at a group holding own, allied and enemy `E1X` and `E2`, with one own `E1X` inside a transport nearby. Expect own and allied `E1X` and `E2` in range to become `E1Y`; enemy ones and the passenger stay. Add `Convert1.From=E1Y`, `Convert1.To=E1Z`: an `E1X` still becomes only `E1Y`. For the superweapon, add `Convert.From=E1X`, `Convert.To=E1Y` to a copy of the Iron Curtain superweapon type (default `AffectsHouse=owner`): at launch every own `E1X` on the map, including the passenger, becomes `E1Y`, wherever the curtain is placed; allied `E1X` do not change.
14. Compare with Ares. Run test 12 on Phobos with the real Ares loaded, and without it if a build is available. Note any difference in health, ammo, turret facing and attached effects. The result sets the target for open question 4.
15. Owner-control conversion. In a skirmish against an AI, `MTNKD` with `Convert.ComputerToHuman=MTNKE`: mind-control an AI-owned one. It becomes `MTNKE`. Give `MTNKE` `Convert.HumanToComputer=MTNKD` and kill the controller: it returns to the AI and becomes `MTNKD`. Edge: take a unit from a human ally (human to human): nothing converts.
16. Infantry deploy. A GI copy `E1D1` with `Deployer=yes`, `Convert.Deploy=E1D2`; `E1D2` with `Deployer=yes`, `Convert.Undeploy=E1D1`. Deploy it: the unit becomes `E1D2` when the Deploy sequence ends (the tooltip name changes), and becomes `E1D1` when the Undeploy sequence ends. Edge: order a deployed `E1D2` to move without undeploying first and record whether it converts (open question 11).
17. Refused conversion. A warhead with `Convert.From=E1X`, `Convert.To=` a vehicle type: nothing changes and the debug log reports incompatible types.

Anim-to-Unit

18. Owner kinds. Give `MTNKG` (Grizzly copy) `DestroyAnim=GISPAWN`, where `GISPAWN` is a copy of a short explosion animation with `CreateUnit=E1`, `CreateUnit.Owner=Victim`, `CreateUnit.RandomFacing=no`, `CreateUnit.Facing=64`. Let an enemy destroy the tank: a GI owned by the tank's owner appears on the wreck cell as the animation ends, facing east, on guard. Change the owner to `Killer`: the GI belongs to the killer. With `Killer`, add `AutoDeath.Behavior=kill`, `AutoDeath.AfterDelay=300` to `MTNKG` and let it die alone: with `CreateUnit.RequireOwner=yes` no GI appears; with `no` the GI belongs to the Civilian-side house.
19. Facing options. With `CreateUnit.RandomFacing=yes` (the default), destroy several tanks: the GIs face different directions. With `CreateUnit.InheritFacings=yes` and `RandomFacing=no`, destroy tanks that face different ways: each GI faces the way its tank faced. With a turreted vehicle as the created unit and `CreateUnit.InheritTurretFacings=yes`, the new turret points where the dead tank's turret pointed.
20. Placement. A weapon whose warhead `AnimList=` is the spawn animation, fired at a building, with `CreateUnit.ConsiderPathfinding=yes`: the unit appears on the nearest free cell. With `no`: it appears on the building's cell. Edge: an anti-air weapon with the same `AnimList` fired at an aircraft, so the animation plays at altitude. With `CreateUnit.AlwaysSpawnOnGround=yes` the unit appears on the ground below; with `CreateUnit.SpawnParachutedInAir=yes` and `AlwaysSpawnOnGround=no` it descends on a parachute.
21. Mission and spawn animation. Destroy one tank owned by the AI and one owned by a human with `CreateUnit.Mission=Guard` and `CreateUnit.AIMission=Hunt`: the AI's GI hunts, the human's guards. `CreateUnit.SpawnAnim=` an explosion animation plays where each GI appears.
22. Multiplayer sync. Two clients play a skirmish on a test map with tests 1, 9, 18 and 19 set up, using `CreateUnit.RandomFacing=yes` and `CreateUnit.Owner=Random`. Both clients end with the same unit facings, owners and counts and report no out-of-sync.
