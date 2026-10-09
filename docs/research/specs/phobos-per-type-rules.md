# Per-type values for global rules keys (Phobos)

Sources: Phobos `docs/Fixed-or-Improved-Logics.md` (sections "Customizable teleport/chrono locomotor settings", "Unit repair customization", "Customize `HarvesterDumpRate`/`HarvesterLoadRate`/`IdleActionFrequency`", "Building bunker/garrison multipliers", "Wake animations", "Landing direction", "Customizable projectile gravity", "Sinking", "Customizable debris & meteor impact") and `docs/New-or-Enhanced-Logics.md` ("Customizable OpenTopped properties", "Customizable disk drain logic", "Overload characteristic dehardcode", "Customize `SellSound`", "Customizable `SlavesFreeSound`", "Engineer repair", "Custom Mind Control Animation", "Custom `SplashList` on Warheads", "Parabombs"). Source files read: `src/Ext/Techno/Hooks*.cpp`, `src/Ext/TechnoType/Body.cpp` and `Hooks.Teleport.cpp`, `src/Ext/BuildingType/Hooks.cpp` and `Body.cpp`, `src/Ext/Building/Hooks.cpp`, `src/Ext/Unit/Hooks.Harvester.cpp`, `src/Ext/Aircraft/Hooks.cpp` and `Body.cpp`, `src/Ext/Bullet/Hooks*.cpp`, `src/Ext/BulletType/Body.cpp`, `src/Ext/WarheadType/Hooks.cpp`, `src/Ext/Anim/Hooks.cpp`, `src/Ext/CaptureManager/Hooks.cpp`, `src/Utilities/Template.h` and `TemplateDef.h` (read at commit e8ec07f).

Each key in this group lets one type replace a value that stock Yuri's Revenge reads once for the whole game. The type's value wins when it is set; otherwise the global value applies, so an unmodified mod behaves as before. The engine already reads five of the keys (the `OpenTopped.*` and `OpenTransport.*` values); the rest are new.

## Keys

"Engine" says what `code/` reads today, checked against `manual/data/ini-keys.yaml` and the source: "key and global" means both are read; "global only" means the engine reads the global value but not the per-type key; "no" means neither is read, so the global key has to be added too. The shared fallback rule is under "Behaviour".

| Key | Type section | Value | Falls back to | Engine |
|---|---|---|---|---|
| `HarvesterLoadRate` | UnitType | integer, frames | `[General] HarvesterLoadRate` | global only |
| `HarvesterDumpRate` | UnitType | float, game minutes | `[General] HarvesterDumpRate` | global only |
| `IdleActionFrequency` | InfantryType | one float, or two integers (min, max frames) | `[AudioVisual] IdleActionFrequency` | global only |
| `SlavesFreeSound` | InfantryType | sound | `[AudioVisual] SlavesFreeSound` | global only |
| `DefaultMirageDisguises` | UnitType | list of TerrainTypes | `[General] DefaultMirageDisguises` | global only |
| `DeployDir` | UnitType | integer 0-7 (one of eight facings), `-1` for no restriction | `[AudioVisual] DeployDir` (a 0-255 facing, divided by 32), only for units with `DeployingAnim` | no |
| `CurleyShuffle` | AircraftType | boolean | `[General] CurleyShuffle` | global only |
| `LandingDir` | AircraftType | integer 0-255; negative has a special meaning | `[AudioVisual] PoseDir` (see Behaviour) | global only |
| `SpawnHeight` | AircraftType | integer, leptons | the type's `FlightLevel`, then `[General] FlightLevel` | no |
| `OpenTopped.RangeBonus` | TechnoType | integer, cells | `[CombatDamage] OpenToppedRangeBonus` | key and global |
| `OpenTopped.DamageMultiplier` | TechnoType | float | `[CombatDamage] OpenToppedDamageMultiplier` | key and global |
| `OpenTopped.WarpDistance` | TechnoType | integer, cells | `[CombatDamage] OpenToppedWarpDistance` | key and global |
| `OpenTransport.RangeBonus` | TechnoType | integer, cells | `[CombatDamage] OpenTransport.RangeBonus` (a Phobos key, default `0`) | key and global |
| `OpenTransport.DamageMultiplier` | TechnoType | float | `[CombatDamage] OpenTransport.DamageMultiplier` (a Phobos key, default `1.0`) | key and global |
| `Wake` | TechnoType | AnimType | `[General] Wake` | global only |
| `Wake.Grapple` | TechnoType | AnimType | the type's `Wake`, then `[General] Wake` | no |
| `Wake.Sinking` | TechnoType | AnimType | the type's `Wake`, then `[General] Wake` | no |
| `MakesWake` | TechnoType | boolean | the global flag for the unit's locomotor: `[AudioVisual] WalkLocomotorMakesWake` (default `no`), `DriveLocomotorMakesWake`, `HoverLocomotionClassMakesWake`, `ShipLocomotionClassMakesWake` (default `yes`) | no |
| `WakeAnim` | AnimType and VoxelAnimType (art) | list of AnimTypes | `[General] Wake`, except for `IsMeteor` | no |
| `ChronoTrigger` | TechnoType | boolean | `[General] ChronoTrigger` | no |
| `ChronoDistanceFactor` | TechnoType | integer | `[General] ChronoDistanceFactor` | no |
| `ChronoMinimumDelay` | TechnoType | integer, frames | `[General] ChronoMinimumDelay` | no |
| `ChronoRangeMinimum` | TechnoType | integer, leptons | `[General] ChronoRangeMinimum` | no |
| `ChronoDelay` | TechnoType | integer, frames | `[General] ChronoDelay` | no |
| `WarpOut` | TechnoType | list of AnimTypes | `[General] WarpOut` | global only |
| `WarpIn` | TechnoType | list of AnimTypes | `[General] WarpIn` | no |
| `Chronoshift.WarpOut` | TechnoType | list of AnimTypes | the type's `WarpOut`, then `[General] WarpOut` | no |
| `Chronoshift.WarpIn` | TechnoType | list of AnimTypes | the type's `WarpIn`, then `[General] WarpIn` | no |
| `WarpAway` | TechnoType | list of AnimTypes | `[General] WarpAway` | global read, not used by any code |
| `DrainMoneyFrameDelay` | TechnoType (the drainer) | integer, frames | `[CombatDamage] DrainMoneyFrameDelay` | global only |
| `DrainMoneyAmount` | TechnoType (the drainer) | integer, credits | `[CombatDamage] DrainMoneyAmount` | global only |
| `DrainAnimationType` | TechnoType (the drainer) | AnimType | `[CombatDamage] DrainAnimationType` | global only |
| `Overload.Count` | TechnoType | list of integers | `[CombatDamage] OverloadCount` | global only |
| `Overload.Damage` | TechnoType | list of integers | `[CombatDamage] OverloadDamage` | global only |
| `Overload.Frames` | TechnoType | list of integers | `[CombatDamage] OverloadFrames` | global only |
| `Overload.DeathSound` | TechnoType | sound | `[AudioVisual] MasterMindOverloadDeathSound` | global only |
| `Overload.ParticleSys` | TechnoType | ParticleSystemType | `[CombatDamage] DefaultSparkSystem` | global only |
| `LaserTargetColor` | TechnoType | integer, `[ColorAdd]` index | `[AudioVisual] LaserTargetColor` | no |
| `ShadowSizeCharacteristicHeight` | TechnoType | integer, leptons | `[JumpjetControls] CruiseHeight` for non-jumpjets, the locomotor height for jumpjets | no |
| `SellSound` | TechnoType (buildings and vehicles) | sound | `[AudioVisual] SellSound` | global only |
| `Grinding.Sound` | BuildingType | sound | `[AudioVisual] EnterGrinderSound` | global only |
| `BuildingRepairedSound` | BuildingType | sound | `[AudioVisual] BuildingRepairedSound` | no |
| `BunkerWallsUpSound` | BuildingType | sound | `[AudioVisual] BunkerWallsUpSound` | global only |
| `BunkerWallsDownSound` | BuildingType | sound | `[AudioVisual] BunkerWallsDownSound` | global only |
| `OccupyDamageMultiplier` | BuildingType | float | `[CombatDamage] OccupyDamageMultiplier` | global only |
| `OccupyROFMultiplier` | BuildingType | float | `[CombatDamage] OccupyROFMultiplier` | global only |
| `BunkerDamageMultiplier` | BuildingType | float | `[CombatDamage] BunkerDamageMultiplier` | global only |
| `BunkerROFMultMultiplier` | BuildingType | float | `[CombatDamage] BunkerROFMultiplier` | global only |
| `Units.RepairRate` | BuildingType | float, game minutes | `[General] URepairRate` (`UnitRepair=yes`), `[General] IRepairRate` (`Hospital=yes`); for `UnitReload=yes` the key replaces the repair step of `[General] ReloadRate` | global only |
| `Units.RepairStep` | BuildingType | integer, strength | the step the docked type uses: `[General] RepairStep`, or `IRepairStep` for infantry | global only |
| `Units.RepairPercent` | BuildingType | float, percent or fraction | `[General] RepairPercent` | global only |
| `Gravity` | BulletType | float | global `Gravity` | global only (read from `[AudioVisual]`) |
| `MissileSafetyAltitude` | BulletType | integer, leptons | `[General] MissileSafetyAltitude` | no |
| `Parachuted.MaxFallRate` | BulletType | integer | `[General] ParachuteMaxFallRate` | global only |
| `BombParachute` | BulletType | AnimType | `[General] BombParachute` | global only |
| `MindControl.Anim` | WarheadType | AnimType | `[CombatDamage] ControlledAnimationType` | global only |
| `SplashList` | WarheadType | list of AnimTypes | `[CombatDamage] SplashList` | global only |
| `Parasite.ParticleSystem` | WarheadType | ParticleSystemType | `[CombatDamage] DefaultSparkSystem` | global only |
| `SplashAnims` | AnimType and VoxelAnimType (art) | list of AnimTypes | `[CombatDamage] SplashList` | global only |

Notes on spelling and sections:

- The Phobos code reads `BunkerROFMultMultiplier` (with the doubled word) and the global flags `HoverLocomotionClassMakesWake` and `ShipLocomotionClassMakesWake`. The Phobos docs spell the last two `HoverLocomotorMakesWake` and `ShipLocomotorMakesWake`, which the code does not read. Use the code spelling, and accept the docs spelling only if the owner wants it.
- The Phobos docs list `BunkerROFMultMultiplier` as defaulting to a global of the same name. No such global exists; the code falls back to `[CombatDamage] BunkerROFMultiplier`.
- `Units.RepairRate` for `Hospital=yes` falls back to `IRepairRate`. The docs mention only `URepairRate` and `ReloadRate`.

Related keys with their own specs: `BallisticScatter.Max` is in `airburst-scatter-zadjust.md`; `Promote.VeteranSound` and the other promotion keys are in `crashable-and-promotion.md`; `Bolt.ParticleSystem` and `Beam.Color` are in `electric-bolt-colours.md`.

Left out of this spec: the `DropPod.*` keys (they belong to the drop-pod feature), radiation and shield type keys that fall back to `[Radiation]` and `[AudioVisual]` globals, the keys whose fallback is a Phobos-defined global with no stock counterpart (`SinkSpeed`, `Sinkable`, `OpenTopped.IgnoreRangefinding` and the rest of that family, `KeepAlive.*`, `ConditionYellow.Terrain`, which is a global and not a per-type key despite its docs entry), `SmallFireAnims` and `LargeFireAnims`, and the warhead `DebrisAnims` (the engine has no warhead debris at all).

## Behaviour

### Shared fallback rule

- An object looks the value up in the type it has at that moment. A unit converted to another type uses the new type's values.
- The lookup happens each time the value is used, so changing a global in a later INI read changes every type that has no value of its own. `LaserTargetColor` is the exception: it is resolved to a colour when the type is read.
- A key is unset when it is missing or has an empty value. The remaining cases depend on the value kind.
- Number, boolean, single AnimType or ParticleSystemType: `none`, an empty value and `<default>` all leave the key unset, so the global applies. A value that does not parse is written to the debug log and leaves the key unset. `Wake=none` therefore does not stop wakes; the global wake still plays.
- Sound: `none` and an empty value set the key to "no sound", which overrides a global sound. `<default>` is reported as an unknown sound and leaves the key unset. A name that matches no sound is reported and ignored.
- Lists that can be unset (`SplashList`, `SplashAnims`, `DefaultMirageDisguises`, `Overload.Count/Damage/Frames`): any value except `<default>` sets the key. `none` gives an empty list, which replaces the global list with nothing. Names that match no type are dropped with a log line, so a list of only bad names also becomes empty.
- Plain lists (`WarpIn`, `WarpOut`, `WarpAway`, `Chronoshift.WarpIn/WarpOut`, `WakeAnim`): an empty list counts as unset, so these cannot be emptied per type.
- Floats are parsed with `strtod`, rounded to single precision, and divided by 100 when the text contains `%`. The engine's `INIClass::Get_Float` (`code/ini.cpp`) also rounds through `strtof` and divides by 100 for `%`, but it divides in double precision where Phobos divides in single precision, so a value such as `33%` can differ in the last bit.
- A per-type key stores nothing in the object. Owner change, death, transport, cloaking, EMP and temporal effects do not alter which value applies; where one of them changes whether the feature runs at all, it is noted below.
- All values come from rules, so every machine in a multiplayer game must load the same rules. None of these keys draws random numbers.

### Harvesters and infantry

- `HarvesterLoadRate` replaces the rate read at the three places the engine uses `Rule->HarvesterLoadRate` (`UnitClass::Harvesting` twice and `UnitClass::Do_MISSION_HARVEST`). The engine multiplies the rate by 3 when harvesting veins; keep that factor on the per-type value.
- `HarvesterDumpRate` is in game minutes; one unload pass lasts `rate * 900` frames. It replaces the read in `UnitClass::Do_MISSION_UNLOAD`. `UnitClass::Queue_Wait_Distance` also reads the global to estimate how long a queued harvester waits; it should use the type of the harvester that is unloading, and each harvester queued behind it.
- `IdleActionFrequency` with one value is the multiplier the global uses: the idle interval is a random number of frames between `value * 450` and `value * 1800`. With two values they are the lower and upper bound in frames, in either order. The engine's use is `InfantryClass::Random_Animate`, which reads `Rule->RandomAnimateTime`.
- `SlavesFreeSound` plays at the freed slave's position, once per freed slave, using the slave's own type. See open question 12.
- `DefaultMirageDisguises` picks one entry at random with the scenario random generator, as stock does, so the list only changes which entries can be picked. An empty list gives no disguise.

### Aircraft

- `CurleyShuffle` is read at four decision points in the attack mission. The engine has the same four in `AircraftClass::Do_MISSION_ATTACK`. Fighters ignore the setting there.
- `LandingDir` takes part in a longer chain that Phobos uses for every landing facing. In order: a spawned aircraft takes its carrier's facing; a building's docking direction (a separate Phobos key) wins when the aircraft docks; otherwise `LandingDir` applies. For an `AirportBound=yes` type the low byte of the value is used even when negative; for others a negative value means "keep the current facing". Without `LandingDir`, a newly produced aircraft uses `PoseDir.Production` (default: `PoseDir` as a 0-255 facing) and an aircraft landing in the field uses `PoseDir.Field` (default: `PoseDir * 32`). The docs say the field default is `8 * PoseDir`; the code multiplies by 32. Stock reads `PoseDir` as one of eight facings in the field, which `* 32` reproduces.
- `SpawnHeight` applies only to reinforcement aircraft created from an airstrike team, spy plane or paradrop superweapon. The aircraft starts at this height, or at its flight level when the key is unset, and faces its target.

### Open-topped transports

- Done in the engine: `TechnoTypeClass::Read_INI` (`code/techtype.cpp`) reads all five keys into `std::optional` members; `TechnoClass::In_Range` adds the transport's `OpenTopped.RangeBonus` and the passenger's `OpenTransport.RangeBonus` in cells; `TechnoClass::Fire_At` multiplies damage by the transport's `OpenTopped.DamageMultiplier` and the passenger's `OpenTransport.DamageMultiplier`; `TemporalClass::Update` lets go of the target when it is farther than the transport's `OpenTopped.WarpDistance`. This is recorded in `manual/changes/phobos-opentopped-values.md`.
- Each of those checks applies only while the passenger is in an open-topped transport. The two multipliers are applied in one product with a single truncation to an integer.
- Not done: reading a blank, `none` or `<default>` value as unset (open question 1), and the other `OpenTopped.*` keys of the same Phobos section, which fall back to Phobos-defined globals and are outside this spec.

### Wakes

- `Wake` replaces `Rule->Wake` at every wake spawned by a moving unit. The engine creates wakes in `DriveLocomotionClass::Process` (ships use it), `HoverLocomotionClass::Process`, `LevitateLocomotionClass::Process`, `AnimClass::AI`, `VoxelAnimClass::AI`, `InfantryClass::Take_Damage` (infantry dying over water), `Chrono_Shift` in `code/super.cpp` and `MapClass::Break_Ice`. Phobos changes the drive, hover, ship and walk locomotors, the parasite grapple and the two sinking sites; it leaves levitate alone (open question 10). The engine has no walk-locomotor wake.
- `Wake.Grapple` applies while a parasite holds the unit on water. `Wake.Sinking` applies while a vehicle sinks. The engine has no call that creates a wake for either case today.
- `MakesWake` decides whether the locomotor spawns wakes at all. A walking unit wakes every 10 frames while it is really moving on a water cell and not on a bridge.
- `WakeAnim` on an AnimType or VoxelAnimType is used when the animation lands in water without `ExplodeOnWater`. An empty list uses `Rule->Wake`, except for `IsMeteor` animations, which make no wake.

### Teleport and chronoshift timing

- After a teleport the unit waits `max(distance / max(ChronoDistanceFactor, 1), ChronoMinimumDelay)` frames when `ChronoTrigger=yes` and the jump is at least `ChronoRangeMinimum` long, and `ChronoMinimumDelay` frames otherwise. Harvesters and weeders skip the wait. `ChronoDelay` is the lock time after arrival.
- The animation lists are tried in this order: `Chronoshift.WarpOut` (only when a Chronosphere moves the unit), then `WarpOut`, then `[General] WarpOut`; the same holds for the `WarpIn` side. A list with several entries plays one chosen at random.
- The `Chrono*` delays and the locomotor's `WarpIn`/`WarpOut` need a timed teleport locomotor first: the engine's `TeleportLocomotionClass` (`code/teleport.cpp`) sets the unit down at once and has none of these delays. `Chronoshift.WarpOut` and `Chronoshift.WarpIn` can be added earlier, because the Chronosphere path in `Chrono_Shift` (`code/super.cpp`) already plays `Rule->WarpOut` itself.

### Drain, overload, visuals

- The three `Drain*` keys are read from the drainer's type each time the drain acts. A negative `DrainMoneyAmount` moves credits from the drainer's owner to the owner of the drained object. A zero amount does nothing. The engine's drain is in `TechnoClass::AI` and `TechnoClass::Start_Drain`, and handles refineries only.
- `Overload.Count`, `Overload.Damage` and `Overload.Frames` are indexed together by the number of controlled units. Phobos counts how many `Overload.Count` entries are below the controlled count (0 up to the list length) and uses that as the index into the damage and frames lists, clamped to their last entry. The engine caps the index at the last entry of the count list and uses 0 when the other lists are shorter (open question 6).
- `Overload.ParticleSys` picks the spark type; the number of sparks is a separate Phobos key.
- `LaserTargetColor` tints the airstrike target; it is the colour of the unit that calls the strike. `ShadowSizeCharacteristicHeight` only matters when Phobos height-based shadow scaling is on, which the engine does not have.

### Sounds

- `SellSound` on a building plays at the building when it is sold, unless the building undeploys into a vehicle. On a vehicle it plays at the vehicle's position, only for the human player who owns it; stock plays it globally.
- `Grinding.Sound` plays at the entering object when set and then replaces the stock grinder sound. When unset the stock `EnterGrinderSound` plays.
- `BuildingRepairedSound` plays when an engineer repairs the building.
- `BunkerWallsUpSound` and `BunkerWallsDownSound` play at the bunker when the walls move; an index of -1 plays nothing.

### Garrison and bunker multipliers

- `OccupyDamageMultiplier` and `OccupyROFMultiplier` replace the global for infantry firing from a garrisoned building. `BunkerDamageMultiplier` and `BunkerROFMultMultiplier` replace the global for a vehicle in a tank bunker; the value is taken from the bunker building's type.
- The engine applies the occupy damage multiplier in `TechnoClass::Fire_At` only when `BuildingClass::Can_Occupy_Fire` is true, and the occupy ROF multiplier in `TechnoClass::Rearm_Delay` after dividing by the occupant count. Keep both conditions. For the occupy ROF multiplier, Phobos and the engine both leave the delay unchanged when the value is 0 or less.

### Unit repair

- `Units.RepairRate` is in game minutes; the interval is `rate * 900` frames. `Units.RepairStep` and `Units.RepairPercent` apply to every kind of repair building (`UnitRepair`, `UnitReload`, `Hospital`) and to infantry, vehicles and aircraft alike.
- Repair cost per step is `cost / (Strength / step) * RepairPercent`, at least 1. `Units.UseRepairCost` (a separate Phobos key, not covered here) turns the cost off; its default is off for infantry and on for everything else. Phobos computes the division in floating point; stock divides integers (open question 8).
- For `UnitRepair=yes` and `Hospital=yes` buildings the key replaces the rate in `BuildingClass::Do_MISSION_REPAIR`.
- For `UnitReload=yes` buildings, setting the key switches the repair off the pad's normal pass. The pad still reloads at `[General] ReloadRate`. Repair then runs on its own timer: every frame where `CurrentFrame % (rate * 900) == 0` (at least every frame), each docked ground unit below full strength that is not moving is repaired one step. All such buildings share the same global frame clock. A negative rate disables repair on the pad.
- The step, percent and cost changes reach the unit through its repair radio message, so `TechnoClass::Receive_Message` has to know which building sent it.

### Projectiles, warheads, animations

- `Gravity` replaces the gravity used while the projectile flies, in aim and range checks for the weapon, and for building missiles. Floating projectiles use half the value. The engine uses `Rule->Gravity` for this in `BulletClass::AI`, `WeaponTypeClass::Init_Max_Speed`, `TechnoTypeClass::In_Range`, the turret pitch code in `TechnoClass::AI`, `BuildingClass::Barrel_Pitch`, `BuildingClass::Do_MISSION_MISSILE` and `Get_Floater_Gravity` (`code/combat.cpp`). Particles and hover bounce also read `Rule->Gravity`; Phobos leaves them on the global value. A gravity of 0 makes projectiles fly backward, which the docs advise against.
- `MissileSafetyAltitude` is checked when a missile with `ROT` of 1 or more loses its target and climbs. The engine has no such climb, so the global key and the behaviour both need adding.
- `Parachuted.MaxFallRate` and `BombParachute` only matter for projectiles that fall on a parachute (Phobos `Parachuted=yes`); the engine's stock parachuted bombs use `ObjectClass::Paradrop` and `ObjectClass::AI`.
- `MindControl.Anim` is the animation attached to a unit taken by this warhead's mind control. The engine shows `Rule->ControlledAnimationType` in `code/capture.cpp`, so the warhead's value has to be passed into that call.
- `Parasite.ParticleSystem` is the spark system created each time a parasite damages a non-infantry victim; the engine does this in `ParasiteClass::Update` with `Rule->DefaultSparkSystem`. Phobos also has `Parasite.DisableParticleSystem`, which is outside this spec.
- `SplashList` on a warhead replaces the list used when a conventional warhead hits water. Without `SplashList.PickRandom` the entry is chosen by damage, one entry per 35 damage, capped at the last. The engine does this in `Combat_Anim`.
- `SplashAnims` on an animation replaces the list for debris and meteor impacts on water. The docs say the last entry is used unless `SplashAnims.PickRandom` is set; the code picks the first entry for a normal animation and the last for a meteor, which matches stock.

## What stock YR does

Each of these settings has one value for the whole game, read from `rules.ini` (`rulesmd.ini`) at the section named under "Falls back to". Every harvester loads and unloads at the same rate, every open-topped transport gives passengers the same range, damage and warp distance, every garrison and bunker multiplies fire the same way, and every unit repair building heals at `URepairRate` or `IRepairRate`. Wakes, splashes, parachutes, drain animations and sounds come from one global entry. Stock has no per-type override for any of them, and reads only the global key.

Several of the behaviours listed above are stock behaviour that has no INI key at all, such as the walk locomotor never making a wake and the teleport delay formula. The Phobos global flags and the `Chrono*` keys expose those.

## Where it hooks in OpenYR

What exists: type data is read in each type's `Read_INI`, kept as members, written by that class's `Serialize`, and used through `Rule->...` in the places listed under Behaviour. `TechnoTypeClass` already uses `std::optional<T>` plus `value_or(Rule->...)` for the open-topped values (`code/techtype.h`, `code/techtype.cpp`, `code/techno.cpp`, `code/temporal.cpp`). The other keys follow the same pattern.

Class and file for each type section:

| Section | Reader and storage | Use sites |
|---|---|---|
| UnitType | `UnitTypeClass::Read_INI`, `code/unittype.cpp` | `code/unit.cpp`: `Harvesting`, `Do_MISSION_HARVEST`, `Do_MISSION_UNLOAD`, `Queue_Wait_Distance`, `Mirage_AI` |
| InfantryType | `InfantryTypeClass::Read_INI`, `code/infatype.cpp` | `InfantryClass::Random_Animate` (`code/infantry.cpp`); `SlaveManagerClass::Free_All` (`code/slaveman.cpp`) |
| AircraftType | `AircraftTypeClass::Read_INI`, `code/airctype.cpp` | `AircraftClass::Do_MISSION_ATTACK` (`code/aircraft.cpp`) |
| TechnoType | `TechnoTypeClass::Read_INI`, `code/techtype.cpp` | `TechnoClass::AI` and `Start_Drain` (`code/techno.cpp`), `CaptureManagerClass::Handle_Overload` (`code/capture.cpp`), the wake sites above |
| BuildingType | `BuildingTypeClass::Read_INI`, `code/builtype.cpp` | `TechnoClass::Fire_At` and `Rearm_Delay`, `BuildingClass::Bunker_Up`, `Grind`, `Do_MISSION_REPAIR` (`code/building.cpp`), `TechnoClass::Receive_Message` for `RADIO_REPAIR` |
| BulletType | `BulletTypeClass::Read_INI`, `code/bullettype.cpp` | `BulletClass::AI` (`code/bullet.cpp`), `Get_Floater_Gravity` (`code/combat.cpp`), `ObjectClass::Paradrop` (`code/object.cpp`) |
| WarheadType | `WarheadTypeClass::Read_INI`, `code/warhead.cpp` | `Combat_Anim` (`code/combat.cpp`), `code/capture.cpp`, `code/parasite.cpp` |
| AnimType, VoxelAnimType | `AnimTypeClass::Read_INI` (`code/animtype.cpp`), `VoxelAnimTypeClass::Read_INI` (`code/vanimtype.cpp`) | `AnimClass::AI` (`code/anim.cpp`), `VoxelAnimClass::AI` (`code/vanim.cpp`) |

Order to build in, smallest first:

1. Add two small helpers beside `CCINIClass::Get_VocType` in `code/ccini.h`: one that returns an `std::optional` for a number, boolean or class pointer (blank, `none` and `<default>` give no value), and one for sounds (`none` gives "no sound"). Existing `Get_VocType` returns its default for `none`, so it cannot express the sound rule.
2. Switch the five open-topped reads in `TechnoTypeClass::Read_INI` to the number helper, which fixes open question 1.
3. Pure lookups that replace one `Rule->` read: `HarvesterLoadRate`, `HarvesterDumpRate`, `CurleyShuffle`, `SlavesFreeSound`, `SellSound`, `BunkerWallsUpSound`, `BunkerWallsDownSound`, `DrainMoneyFrameDelay`, `DrainMoneyAmount`, `DrainAnimationType`, `Wake` in the drive and hover locomotors.
4. The four garrison and bunker multipliers in `Fire_At` and `Rearm_Delay`.
5. `Units.RepairRate`, `Units.RepairStep` and `Units.RepairPercent`: pass the sending building to the repair code and add the pad's own repair timer.
6. List and animation keys: `SplashList`, `SplashAnims`, `MindControl.Anim`, `Parasite.ParticleSystem`, `DefaultMirageDisguises`, `Overload.*`, `IdleActionFrequency`.
7. Projectile keys: `Gravity` at all gravity sites, `Parachuted.MaxFallRate`, `BombParachute`, `MissileSafetyAltitude` (with the missile climb itself).
8. Keys that need a missing global or feature first: `DeployDir`, `LaserTargetColor`, `ShadowSizeCharacteristicHeight`, the `Chrono*` and `Warp*` keys, `Wake.Grapple`, `Wake.Sinking`, `MakesWake`, `WakeAnim`, `LandingDir`, `SpawnHeight`, `Grinding.Sound`, `BuildingRepairedSound`.

## Saved state

- No per-object, per-house or network state. Every field is rules data.
- New per-type members go into the owning class's `Serialize` (`TechnoTypeClass::Serialize`, `BuildingTypeClass::Serialize`, `UnitTypeClass::Serialize`, `InfantryTypeClass::Serialize`, `AircraftTypeClass::Serialize`, `BulletTypeClass::Serialize`, `WarheadTypeClass::Serialize`, `AnimTypeClass::Serialize`, `VoxelAnimTypeClass::Serialize`). `SaveStreamClass` already writes `std::optional` as a flag plus the value, as it does for the open-topped members. Class pointers use the same swizzled `stream.Serialize` as `ExpireAnim`.
- New globals (`MissileSafetyAltitude`, `Chrono*`, `WarpIn`, `DeployDir`, `LaserTargetColor`, the `MakesWake` flags, `BuildingRepairedSound`) go into `RulesClass::Serialize` in `code/rules.cpp`.
- Every added member changes the content layout, so raise `SaveVersionInfo::REVISION` in `code/savever.h` (see `docs/SAVE-FORMAT.md`, "Versions").
- The type checksum `TechnoTypeClass::Compute_CRC` does not include the existing open-topped members. Whether the new ones that change the simulation should be added is open question 13.

## Open questions

1. The engine reads the open-topped keys with `Is_Present` and `Get_Int` with a default of 0 (`code/techtype.cpp`), so `OpenTopped.RangeBonus=` (empty), `=none` and a non-numeric value all become 0 and override the global. Phobos treats them as unset and uses the global. The engine also does not understand `<default>`. Decide whether to match Phobos; the fix is step 2.
2. Sound keys: Phobos lets `SellSound=none` silence one type. The engine's `Get_VocType` turns `none` back into the global. The helper in step 1 follows Phobos; confirm the owner wants that.
3. Phobos bug: the damage hook for a bunkered vehicle (`TechnoClass_FireAt_BunkerDamageBonus`) falls back to `OccupyDamageMultiplier` when the bunker has no `BunkerDamageMultiplier`, while the firepower function used elsewhere falls back to `BunkerDamageMultiplier`. With stock values both are 1.0, so it shows only in mods that set them apart. Decision: follow the sensible reading (`BunkerDamageMultiplier`), or reproduce Phobos? Test 5 shows what Phobos does.
4. Bunker rate multiplier: the engine divides the delay by it whenever it is not 0 (`TechnoClass::Rearm_Delay`), so a negative value gives a negative delay. Phobos divides only when the value is above 0. Keep the engine's rule or Phobos's?
5. The Phobos occupy-damage hook runs for any building that reaches it and does not re-test `Can_Occupy_Fire`. Whether the stock instructions it replaces already made that test is unknown; the engine tests it explicitly.
6. Overload lists: for a controlled count above every `Overload.Count` entry, Phobos uses index = list length (so it needs one more damage and frames entry than count entries, or it reuses the last one). The engine stops at the last count entry and uses 0 when the damage or frames list is short (`CaptureManagerClass::Handle_Overload`). Phobos does this for global lists as well as per-type lists. Run test 12 on the real game with and without Phobos to see which index stock uses.
7. Drain: the Phobos hook that replaces the credit step does not test for a refinery; the original code it skips may have. The engine limits draining to refineries. A zero `DrainMoneyFrameDelay` divides by zero in Phobos; the engine already skips the drain when the delay is 0 or less, and should do the same for a per-type value.
8. `Units.RepairPercent`: Phobos always computes the repair cost with floating-point division for building-sourced repair, even when its `FixRepairStepCost` flag is off. Stock (and the engine's `TechnoTypeClass::Repair_Cost`) divides integers. Decision: use the old formula whenever `Units.RepairPercent` is unset, so unchanged mods keep their costs. Also unclear: what a negative `Units.RepairRate` does on a `UnitRepair` or `Hospital` building (the compare `stage >= rate * 900` is then always true).
9. `Gravity`: the engine reads the global from `[AudioVisual]` with a default of 3, while the Phobos examples use 6 and stock keeps `Gravity` in `[General]` (to be confirmed against the stock rules file). Also, the weapon's projectile speed is worked out from its range and the global gravity when rules load (`WeaponTypeClass::Init_Max_Speed`); a per-projectile gravity needs that speed recomputed, or shells fall short or long at maximum range.
10. `MakesWake` and levitate: the engine's `LevitateLocomotionClass::Process` spawns a wake; Phobos has no flag for that locomotor. Should `MakesWake` and `Wake` apply to it?
11. `LandingDir` and the `PoseDir` defaults: docs and code disagree on the field default (`8 * PoseDir` against `PoseDir * 32`). The code value reproduces stock. Also `ShadowSizeCharacteristicHeight`: the docs say the default is the cruise height (`JumpjetHeight`, `FlightLevel`); the code uses the locomotor height for jumpjets and `[JumpjetControls] CruiseHeight` for everything else, never `FlightLevel`.
12. `SlavesFreeSound`: the engine plays one sound at the slave miner when any slave is freed (`SlaveManagerClass::Free_All`); the Phobos hook sits inside a per-slave step. Whether stock plays one sound or one per slave decides which type's key is read when slaves of several types are freed.
13. Whether any of the new keys should be added to `Compute_CRC` of their type. Phobos does not do it, and the open-topped members are missing from it today.
14. Name collisions: `DebrisAnims` exists in the engine as an Ares-style TechnoType key (`code/techtype.cpp`) and in Phobos as a WarheadType key with a different meaning. Keep them apart in code and in the manual.

## Test plan

All tests use a test mod that sets the relevant globals explicitly, so a stock default cannot hide the effect. Use a skirmish with a short unit and structure list, and count frames from the in-game frame counter or with a stopwatch at one fixed game speed. Run each test twice: with the key and with the key deleted. Define test weapons yourself so damage numbers are exact: a `TestGun` with `Damage=10`, `ROF=60`, `Range=10`, `Burst=1`, a projectile with `Inviso=yes`, and a warhead `TestWH` with `Verses=100%,100%,100%,100%,100%,100%,100%,100%,100%,100%,100%` and `Wall=no`.

1. Harvester dump rate (distinguishes from stock). `[General] HarvesterDumpRate=0.016`. Give `HARV` `HarvesterDumpRate=0.1` and leave `CMIN` unchanged. Fill each with one ore type and unload at an ore refinery. Expected: `CMIN` takes about 30 frames to unload (two passes of 15), `HARV` about 180 (two passes of 90), a ratio of six. With the key removed both take 30.
2. Fallback by blank value (edge). Set `HarvesterDumpRate=<default>` on `HARV`, then repeat with an empty value, and with `HarvesterDumpRate=abc`. Expected in all three: `HARV` unloads in 30 frames, the global rate. Phobos writes a log line only for `abc`.
3. Garrison damage. `[CombatDamage] OccupyDamageMultiplier=1.0`. Make two buildings with `CanBeOccupied=yes`, `CanOccupyFire=yes`, `MaxNumberOccupants=1`; give building A `OccupyDamageMultiplier=3.0` and leave B alone. Garrison each with one infantry whose `Primary` and `OccupyWeapon` are both `TestGun`. Shoot at a target with `Strength=1000` and `Armor=none`. Expected: one shot from A removes 30 strength, one from B removes 10. With the key removed A removes 10 too.
4. Bunker rate multiplier. `[CombatDamage] BunkerROFMultiplier=1.0`. Two tank bunkers, one with `BunkerROFMultMultiplier=2.0`. Park the same tank type in each and shoot at a dummy. Expected: the tank in the first bunker fires at twice the rate (every `ROF/2` frames, plus the random 0-2 frame delay) of the other.
5. Bunker damage fallback (checks open question 3). `[CombatDamage] BunkerDamageMultiplier=2.0`, `OccupyDamageMultiplier=1.0`; a tank bunker with no per-type multipliers holds a tank armed with `TestGun`. Expected from the stock rule: 20 damage per shot. If Phobos is installed and shows 10, the bunker hook falls back to the occupy value and open question 3 is confirmed.
6. Pad repair. `[General] URepairRate=0.016`, `RepairStep=5`. Set `Units.RepairRate=0.1` on a service depot. Dock a vehicle at 50% strength. Expected: strength rises by 5 about every 90 frames at the depot with the key and every 15 frames at a depot without it. Edge: on an airfield (`UnitReload=yes`), set `Units.RepairRate=-1` and dock a damaged aircraft with spent ammo. Expected: it reloads at `ReloadRate` and its strength never rises.
7. Open-topped blank value (edge, already in the engine). On an `OpenTopped=yes` transport set `OpenTopped.RangeBonus=` (empty) with `[CombatDamage] OpenToppedRangeBonus=3`. Load a passenger whose weapon has `Range=5` and give it a target 7 cells away. Expected from Phobos: the passenger fires (range 5+3 cells). Expected from the engine today: it does not fire, because the empty value became 0. This test fails until open question 1 is fixed.
8. Sound override and `none`. `[AudioVisual] SellSound=` set to a clearly audible sound. Give building A `SellSound=none` and building B a different sound. Sell A, B and a third building with no key. Expected: A is silent, B plays its own sound, the third plays the global one. An unknown sound name on a fourth building plays the global one and writes a log line.
9. Wake. `[General] Wake=` set to a visible stock animation. Give `DEST` `Wake=SMOKEY`. Move a `DEST` and another ship across water. Expected: every 10 frames the destroyer leaves a SMOKEY and the other ship leaves the global wake. Edge: set `Wake=none` on the destroyer. Expected: it still leaves the global wake.
10. Drain with a negative amount. `[CombatDamage] DrainMoneyFrameDelay=30`, `DrainMoneyAmount=30`. Give `DISK` `DrainMoneyAmount=-50`. Let it drain an enemy ore refinery while both players hold at least 500 credits. Expected: every 30 frames the Disc's owner loses 50 credits and the refinery's owner gains 50, the reverse of stock, where the refinery's owner pays 30.
11. Warhead splash list. `[CombatDamage] SplashList=` set to the stock splash animations. Give `TestWH` (with `Conventional=yes`) `SplashList=SMOKEY` and fire at a water cell. Expected: SMOKEY plays at the impact. Repeat with `SplashList=none`. Expected: no splash animation (Phobos). With the key removed the global splash plays.
12. Overload lists. Set `[CombatDamage] OverloadCount=2,5`, `OverloadFrames=30,20,10`, `OverloadDamage=10,20,30`. Hold six units with an `InfiniteMindControl` type (above both count entries). Expected from Phobos: each hit is 30 damage and the next hit comes 10 frames later (index 2). Expected from the engine's current rule: 20 damage and 20 frames (index 1). Run it once on the real game without Phobos as well; that run shows the stock index. Then give the type `Overload.Damage=5,6,7` and `Overload.Frames=40`. Expected from Phobos: 7 damage every 40 frames (the frames list is clamped to its only entry).
