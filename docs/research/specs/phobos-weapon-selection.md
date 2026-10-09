# Weapon selection and targeting controls (Phobos)

Sources: Phobos `docs/New-or-Enhanced-Logics.md` (sections "Forcing specific weapon against certain targets", "Disabling fallback to (Elite)Secondary weapon", "Multi Weapon", "No Ammo Weapons", "Targeting limitation for berzerk technos", "Attack non-threatening structures (Techno)" and "(Weapon)", "Extra threat") and `docs/Fixed-or-Improved-Logics.md` (sections "Allow disable an over-optimization in targeting", "Customizable target evaluation map zone check behaviour", "Target scanning delay optimization", "Force techno targeting in distributed frames to improve performance"). Source files: `src/Ext/Techno/Hooks.Firing.cpp`, `src/Ext/Techno/WeaponHelpers.cpp`, `src/Ext/TechnoType/Body.cpp`, `src/Ext/TechnoType/Hooks.MultiWeapon.cpp`, `src/Ext/Techno/Hooks.TargetEvaluation.cpp`, `src/Ext/Techno/Hooks.Targeting.cpp`, `src/Ext/Techno/Hooks.cpp`, `src/Ext/Techno/Body.cpp`, `src/Ext/Techno/Body.Update.cpp`, `src/Ext/Rules/Body.cpp`.

## Keys

All keys are read from `rules.ini` (`rulesmd.ini`). Weapon indices are 0-based: 0 is `Primary`, 1 is `Secondary`, and `Weapon<n>` has index n-1.

### TechnoType section

| Key | Value type | Default |
|---|---|---|
| `ForceWeapon.Naval.Decloaked` | integer weapon index, -1 disables | `-1` |
| `ForceWeapon.Cloaked` | integer weapon index | `-1` |
| `ForceWeapon.Disguised` | integer weapon index | `-1` |
| `ForceWeapon.UnderEMP` | integer weapon index | `-1` |
| `ForceWeapon.InRange` | list of weapon indices | empty |
| `ForceWeapon.InRange.Overrides` | list of floats, cells | empty |
| `ForceWeapon.InRange.TechnoOnly` | boolean | `[General]` value |
| `ForceWeapon.InRange.ApplyRangeModifiers` | boolean | `[General]` value |
| `ForceAAWeapon.InRange` | list of weapon indices | empty |
| `ForceAAWeapon.InRange.Overrides` | list of floats, cells | empty |
| `ForceAAWeapon.InRange.ApplyRangeModifiers` | boolean | `[General]` value |
| `ForceWeapon.Buildings`, `.Defenses`, `.Infantry`, `.Naval.Units`, `.Units`, `.Aircraft` | integer weapon index | `-1` each |
| `ForceAAWeapon.Infantry`, `.Units`, `.Aircraft` | integer weapon index | `-1` each |
| `NoSecondaryWeaponFallback` | boolean | `no` |
| `NoSecondaryWeaponFallback.AllowAA` | boolean | `no` |
| `AllowWeaponSelectAgainstWalls` | boolean | `[CombatDamage]` value |
| `MultiWeapon` | boolean | `no` |
| `MultiWeapon.IsSecondary` | list of weapon indices | empty |
| `MultiWeapon.SelectCount` | integer | `2` |
| `NoAmmoWeapons` | list of weapon indices | empty |
| `NoAmmoWeapons.IgnoreNeverUse` | boolean | `yes` |
| `AlwaysConsideredThreat` | boolean | `no` |
| `ExtraThreat.IsThreat`, `ExtraThreat.InRange` | float | `[General]` value |
| `ExtraThreatCoefficient.InRangeDistance`, `.Facing`, `.DistanceToLastTarget` | float | `[General]` value |
| `TargetZoneScanType` | `same`, `any` or `inrange` | `same` |
| `AINormalTargetingDelay`, `PlayerNormalTargetingDelay` | integer frames | unset (stock `NormalTargetingDelay`) |
| `AIGuardAreaTargetingDelay`, `PlayerGuardAreaTargetingDelay` | integer frames | unset (stock `GuardAreaTargetingDelay`) |
| `AIAttackMoveTargetingDelay`, `PlayerAttackMoveTargetingDelay` | integer frames | unset (stock `NormalTargetingDelay`) |
| `DistributeTargetingFrame` | boolean | `[General]` value |

`NoAmmoWeapons` is only useful together with the Ares keys `NoAmmoWeapon` (integer, default `-1`) and `NoAmmoAmount` (integer, default `0`), which Phobos reads in the same section. The engine reads neither today.

### WeaponType section

| Key | Value type | Default |
|---|---|---|
| `AttackNoThreatBuildings` | boolean | unset (uses the `[General]` key that matches the firer's owner) |

### `[General]`

| Key | Value type | Default |
|---|---|---|
| `ForceWeapon.InRange.TechnoOnly` | boolean | `yes` |
| `ForceWeapon.InRange.ApplyRangeModifiers` | boolean | `no` |
| `ForceAAWeapon.InRange.ApplyRangeModifiers` | boolean | `no` |
| `AutoTarget.NoThreatBuildings` | boolean | `no` |
| `AutoTargetAI.NoThreatBuildings` | boolean | `yes` |
| `DisableOveroptimizationInTargeting` | boolean | `no` |
| `ExtraThreat.IsThreat`, `ExtraThreat.InRange`, `ExtraThreatCoefficient.InRangeDistance`, `ExtraThreatCoefficient.Facing`, `ExtraThreatCoefficient.DistanceToLastTarget` | float | `0.0` each |
| `AINormalTargetingDelay`, `PlayerNormalTargetingDelay`, `AIGuardAreaTargetingDelay`, `PlayerGuardAreaTargetingDelay`, `AIAttackMoveTargetingDelay`, `PlayerAttackMoveTargetingDelay` | integer frames | unset |
| `DistributeTargetingFrame` | boolean | `no` |
| `DistributeTargetingFrame.AIOnly` | boolean | `yes` |

### `[CombatDamage]`

| Key | Value type | Default |
|---|---|---|
| `AllowWeaponSelectAgainstWalls` | boolean | `no` |
| `BerzerkTargeting` | house list: `none`, `owner`/`self`, `allies`/`ally`, `team`, `enemies`/`enemy`, `neutral`, `all` | `all` |

Docs and code disagree on three points:

- The docs put `BerzerkTargeting` under `[General]` and `phobos-tags.md` repeats that. The code reads it from `[CombatDamage]`.
- The docs say `MultiWeapon.IsSecondary` defaults to "Weapon1". The code treats every index except 0 as secondary when the list is empty.
- The docs say `ForceWeapon.InRange` falls back to the weapon's own `Range`. The code does so only when an override or `ApplyRangeModifiers` is set (see Open questions 4).

## Behaviour

### Order of weapon choice

Phobos replaces parts of `TechnoClass::SelectWeapon`. In address order inside that function, the checks run as follows. The first one that returns an index ends the choice.

1. The target is a bullet and the techno is an interceptor: the interceptor's weapon (not part of this spec).
2. With `MultiWeapon=yes`, the stock rule "a type with turrets fires the weapon of its current turret" is skipped, unless the type is a UnitType with `Gunner=yes`.
3. Cell target (force fire on ground): the secondary is used when it exists and the primary cannot be used on that cell, unless `NoSecondaryWeaponFallback=yes` and the ammo rule below does not allow it. A cell that holds an intact wall goes to the wall rule. Both depend on weapon-level `CanTarget` filters that belong to another spec.
4. Ammo rule. If the type has finite `Ammo` and current ammo is at or below `NoAmmoAmount`: the first usable index in `NoAmmoWeapons`, else `NoAmmoWeapon` if it is 0 or higher.
5. `ForceWeapon.*` (first match wins), then the `MultiWeapon` choice.
6. Gattling pairs.
7. A first weapon with a locomotor warhead hands buildings to the second.
8. The primary/secondary decision, with `NoSecondaryWeaponFallback`.
9. The anti-air and underground step: the secondary wins when only it can hit an air or underground target.

Every consumer of the weapon index sees the result: the attack cursor, range checks, target scoring, voice lines and firing.

### ForceWeapon

- Each key is skipped when it is -1. A type with every `ForceWeapon.*` key unset skips the whole step. The step does not run recursively: while `ForceWeapon.InRange` selects a weapon, the force step is off for the nested default selection.
- Checks run in this order, and the first that yields an index wins:
  1. `ForceWeapon.Naval.Decloaked`: target type has `Cloakable=yes` and `Naval=yes` and is currently uncloaked.
  2. `ForceWeapon.Cloaked`: target is cloaked.
  3. `ForceWeapon.Disguised`: target is disguised.
  4. `ForceWeapon.UnderEMP`: target is under EMP.
  5. `ForceAAWeapon.InRange` or `ForceWeapon.InRange` (below).
  6. The target-type keys.
- Target-type keys apply only to technos. Buildings use `ForceWeapon.Defenses` when the target's `BuildCat=Combat` and the key is set, else `ForceWeapon.Buildings`. Infantry and aircraft use the `ForceAAWeapon.*` key when it is set and the target is in the air, else the `ForceWeapon.*` key. Vehicles use `ForceAAWeapon.Units` for an air target when set, then `ForceWeapon.Naval.Units` for a `Naval=yes` target when set, then `ForceWeapon.Units`.
- `ForceWeapon.InRange` runs when the target is a techno, or when it is a cell and `ForceWeapon.InRange.TechnoOnly=no`. It is skipped when both lists are empty. An air target uses the `ForceAAWeapon.InRange` lists if that list is not empty; any other target uses the `ForceWeapon.InRange` lists.
- For each position i, the forced range is `Overrides[i]` cells (times 256 leptons, truncated) when that value is above 0. Otherwise it is the weapon's `Range`, after range modifiers when `ApplyRangeModifiers=yes`. Range modifiers are the veteran range bonus and attached-effect range changes. Distance is centre to centre in leptons. The first position whose weapon index is 0 or higher and whose range covers the target forces that index.
- A position with index -1 and a range that covers the target ends the search without forcing anything, so the default choice applies. This is how an override reserves an inner band for the default weapon.
- The forced index is not checked against the type's weapons. An index that has no weapon makes the techno unable to fire at that target.
- Elite status needs no handling: the index picks the elite weapon automatically when the techno is elite.

### NoSecondaryWeaponFallback

- Applies where weapon choice reaches step 8. Gattling types, `Gunner=yes` multi-turret types and `MultiWeapon` types return earlier, except a `MultiWeapon` type with exactly two considered weapons and the key off, which runs step 8 unchanged.
- With `NoSecondaryWeaponFallback=yes` the techno uses weapon 0 against every target, with these exceptions:
  - `NoSecondaryWeaponFallback.AllowAA=yes`, the target is in the air and the secondary's projectile is `AA=yes`: stock selection runs.
  - Ammo is at or below `NoAmmoAmount` and `NoAmmoWeapon` is 1 or -1: stock selection runs.
  - Every special case that appears earlier in the order above still applies: open-topped `OpenTransportWeapon`, deployed `DeployFireWeapon`, `DrainWeapon` against an enemy `Drainable` building, `ElectricAssault` against an allied `Overpowerable` building, an overpowered building, and a locomotor warhead against a building.
- Stock rules that run after the no-fallback return are therefore skipped: choosing the other weapon when one has 0% `Verses` against the target's armor, `NavalTargeting`, `LandTargeting=2`, and the Phobos weapon filters on the primary. The docs do not mention this.
- With an active shield on the target, the same no-fallback conditions decide whether the secondary may be used when the shield blocks the primary's warhead.
- `AllowWeaponSelectAgainstWalls` applies to a cell with a wall that is not yet destroyed, whether a force-fire order, the cursor or a unit checking if it can clear a wall. If the flag is on (type value, else `[CombatDamage]`) and the type is not a multi-turret non-gattling type: the primary is used when its warhead damages the wall (`Wall=yes`, or `Wood=yes` against a wood wall). If it does not, or the ammo rule applies, the secondary is used when its warhead damages the wall and `NoSecondaryWeaponFallback` is off. Otherwise the primary is used.

### MultiWeapon

- `MultiWeapon=yes` makes the type read `Weapon<n>`, `EliteWeapon<n>` and the matching art FLH keys for n up to `WeaponCount`, as turreted types already do, but without requiring turrets or `Gunner`. Burst-specific FLH keys follow the same rule. If the setting differs from the previous INI layer, weapons 0 and 1 are cleared before reading.
- Selection applies only when `MultiWeapon=yes` and the type is not gattling and not a `Gunner=yes` multi-turret type. Those types keep their stock rules.
- Weapons considered: `min(WeaponCount, SelectCount)` lowest indices. Higher indices remain usable through `ForceWeapon.*` and `NoAmmoWeapons`. Fewer than 2 considered weapons always give index 0. Exactly 2 considered weapons without `NoSecondaryWeaponFallback` give the stock two-weapon rules.
- Secondary weapons are those in `MultiWeapon.IsSecondary` (indices out of range are ignored). With an empty list every index except 0 is secondary. The same set decides which weapons count as secondary for infantry secondary-fire sequences and vehicle firing-sync frames.
- With a techno target, each secondary in index order is preferred over the others in these cases. It is chosen if it can fire at the target:
  - the target is on water and `NavalTargeting` selects the secondary (`Naval_Weapon` result 1): any secondary;
  - the weapon's warhead is `Airstrike`;
  - the target is in the air (see Open questions 7);
  - the weapon has `DrainWeapon`, the target is an undrained `Drainable` techno and not an ally;
  - the weapon's warhead is `ElectricAssault` and the target is an allied `Overpowerable` building.
  A secondary that is missing or has `NeverUse=yes` is skipped.
- If no preferred secondary fires, the first weapon in index order that can fire at the target is chosen, primary first. If none can, index 0. With a cell target and `NoSecondaryWeaponFallback=yes`, index 0.
- "Can fire" here ignores range, ammo and reload. It rejects: `NeverUse` (unless ignored), a non-`FireInTransport` weapon in an open-topped transport, a non-AA projectile against an air target, an `AAOnly` projectile against a ground target, `ElectricAssault` against anything but an allied `Overpowerable` building, a locomotor warhead against a building, Phobos weapon filters, bomb and disarm warheads against the wrong target, `Verses` of 0 against the armor, mind control against an immune or already controlled target, parasite warheads against buildings or victims already eaten, `DrainWeapon` against a non-`Drainable` or allied target, airstrike warheads against ineligible targets, and walls or terrain the warhead cannot damage.
- Scan inputs change too. The threat flags a scan wants are the union over all considered weapons, per rank. Combat damage (used to detect healers and in scoring) is the average over all considered weapons per rank. The guard range, when `GuardRange` is 0, is the longest considered weapon range. Healer scan range uses the longest healing weapon.
- `VoiceWeapon<n>Attack` keys exist only on `MultiWeapon` types and are covered in the voice spec.

### NoAmmoWeapons

- Runs when `Ammo` is 0 or higher and current ammo is at or below `NoAmmoAmount`. It tries each index in `NoAmmoWeapons` in listed order and takes the first weapon that "can fire" (as defined under MultiWeapon). `NoAmmoWeapons.IgnoreNeverUse=yes` (the default) lets a `NeverUse=yes` weapon pass.
- If no listed weapon fits, `NoAmmoWeapon` is used when it is 0 or higher. If that is -1 too, selection continues with the later steps.
- `NoSecondaryWeaponFallback` treats "ammo at or below `NoAmmoAmount`, `Ammo` above 0, and `NoAmmoWeapon` equal to 1 or -1" as permission to use the secondary.

### BerzerkTargeting

- While a techno is berzerk, `CanFire` refuses a shot at a techno target whose owner the setting does not allow. Houses are classed relative to the firer's owner: the same house, a neutral house, an ally, or an enemy. `owner` matches the same house only; `allies` matches allies other than the owner; `team` is both.
- The refusal is the same exit Phobos uses for other "this weapon cannot target that" checks. Target scanning is unchanged: a berzerk techno still picks allies as candidates, so it can hold a target it will not shoot.
- The setting has no effect on non-technos (cells) and does not alter who can be driven berzerk.

### Attack non-threatening structures

- A non-threatening structure is one the stock scan skips for human-owned units outside a team: a building with no turret weapon, or one whose weapon has no range.
- `AutoTarget.NoThreatBuildings=yes` lets human-owned technos pick such structures during automatic targeting. `AutoTargetAI.NoThreatBuildings` does the same for non-human houses. With `AutoTargetAI.NoThreatBuildings=no`, computer-owned technos skip these structures; stock YR never applies that limit to them.
- `AttackNoThreatBuildings` on the weapon the firer would use against the building overrides both keys, in either direction.
- Team members, engineers and buildings that can undeploy into a vehicle keep the stock exemptions. Explicit orders and the attack cursor are unaffected.

### ExtraThreat

- The bonuses are added to the stock threat score of each candidate, once per candidate, for the scanning techno. A type whose five effective values are all 0 skips the code.
- `ExtraThreat.IsThreat`: added when the candidate could fire at the scanner. The candidate counts if its type has `AlwaysConsideredThreat=yes`, or if its weapon against the scanner exists and its fire check, ignoring range, is not `ILLEGAL` (for a building, also not `RANGE`). `AlwaysConsideredThreat` has no effect unless the scanner's effective `IsThreat` is not 0.
- `ExtraThreat.InRange` plus `ExtraThreatCoefficient.InRangeDistance` times the distance in cells: added only when the candidate is within range of the scanner's chosen weapon.
- `ExtraThreatCoefficient.Facing` times the shortest angle between the scanner's firing facing and the bearing from the scanner to the candidate (0 to 32768 on a 65536 circle). The firing facing is the turret facing for turreted vehicles and aircraft, and the body facing for buildings and turretless vehicles. Infantry get no facing bonus.
- `ExtraThreatCoefficient.DistanceToLastTarget` times the distance in cells between the candidate and the scanner's last target position. The position is refreshed every frame while the scanner has a target. It is cleared 45 frames after the scanner loses its target. With no recorded position the term is 0.
- Coefficients are floats and the sum is truncated to an integer when the engine stores the score, so values below about `0.01` can vanish against the `100000` base score in small angle or distance differences.

### TargetZoneScanType

- Stock scans skip a candidate whose movement zone differs from the scanner's. In OpenYR this applies to non-aircraft, non-building scanners in scans that are not range-limited (`Greatest_Threat_Scan` passes the scanner's zone to the cell and candidate tests).
- Phobos reads the scanner's `TargetZoneScanType` at the start of every `GreatestThreat` call and applies it at that zone test. `same` is the stock behaviour. `any` accepts the candidate whatever its zone. `inrange` accepts it when the zones match or when a cell that the scanner's `SpeedType` and `MovementZone` could use near the candidate lies within the selected weapon's range of the candidate; the scanner's own position is not part of that test.
- Aircraft scanners always pass. The same setting also drives the Phobos script attack actions, which are outside this spec.

### DisableOveroptimizationInTargeting

- Stock ring scans return as soon as they have found any object at the end of the ring that is a quarter, or a half, of the way out. The setting skips both early returns, so the scan walks every ring up to the scan radius.
- The docs say this makes units stop shooting nearby targets while ignoring more threatening ones farther away. That holds only if the scan keeps the highest-scoring candidate (Open questions 1).

### Targeting delay and distribution

- Stock YR sets a timer after each scan. The next scan waits `NormalTargetingDelay` frames, or `GuardAreaTargetingDelay` frames when the mission is Area Guard, plus a random 0 to 2 frames from the synchronized random generator.
- The Phobos keys replace the base value per type, separately for human-owned and computer-owned technos, and add a third pair for attack-move (which the engine does not have). The order of use is: attack-move scans first, then Area Guard, then everything else. An unset type key falls back to the matching `[General]` key, then to the stock value.
- `DistributeTargetingFrame=yes` starts the scan timer at creation with 45 plus a per-object random number from 0 to 15. Phobos draws that number for every techno at creation, whatever the setting. It skips human-owned technos when `DistributeTargetingFrame.AIOnly=yes`.
- Phobos also stops the scan for technos that have no weapon.

### Cheap switches and changes to evaluation order

| Keys | Class | Effect on target evaluation |
|---|---|---|
| `BerzerkTargeting` | Cheap switch | One extra refusal in the fire check. Candidate order unchanged. |
| `AutoTarget.NoThreatBuildings`, `AutoTargetAI.NoThreatBuildings`, `AttackNoThreatBuildings` | Cheap switch | Admits or rejects one class of candidate. Scores unchanged. |
| `NoSecondaryWeaponFallback`, `.AllowAA`, `AllowWeaponSelectAgainstWalls` | Cheap switch | One branch in weapon choice. Indirectly changes the weapon that scoring and range tests assume. |
| `ForceWeapon.*` without `InRange` | Cheap switch | One branch at the start of weapon choice, with the same indirect effect. |
| `TargetZoneScanType=any` | Cheap switch | Skips the zone test. Admits many more candidates per scan. |
| `ForceWeapon.InRange`, `ForceAAWeapon.InRange` | Medium | A nested weapon choice and a range check per call, and calls happen several times per candidate. |
| `NoAmmoWeapons` | Medium | Needs the Ares ammo keys first, then a legality check per listed weapon. |
| `TargetZoneScanType=inrange` | Medium | A nearest-cell search per candidate in another zone. |
| `DisableOveroptimizationInTargeting` | Changes evaluation order | Removes the two early returns. Cost grows with the scan radius, and the returned candidate changes (Open questions 1). |
| `ExtraThreat.*`, `AlwaysConsideredThreat` | Changes evaluation order | Adds to every candidate's score. A fire check per candidate when `IsThreat` is used. |
| `TargetingDelay` keys, `DistributeTargetingFrame` | Changes timing | Changes when scans happen, not what they pick. |
| `MultiWeapon.*` | Large | Replaces weapon choice, scan threat flags, damage averages and guard range for the type. |

## What stock YR does

- `SelectWeapon` returns index 0 or 1 for types without turrets, applying the rules listed in order step 8: the secondary is chosen when the primary's warhead has 0% `Verses` against the target, when the secondary is anti-air against an air target, by `NavalTargeting` and `LandTargeting`, and in the special cases the docs list. Turret types fire their current turret's weapon; gattling types fire the pair for their stage.
- `NoSecondaryWeaponFallback`, `MultiWeapon` and every `ForceWeapon.*` key do not exist. Ares `NoAmmoWeapon` and `NoAmmoAmount` select one weapon at low ammo.
- A berzerk techno fires at any house.
- Human-owned technos outside a team never pick unarmed structures on their own. Computer-owned technos do.
- Scanners skip candidates in another movement zone in zone-checked scans, with no way to relax it.
- Ring scans return early at a quarter and at half of the radius once they hold an object.
- A candidate's score uses the type's five threat coefficients and the 100000 base only.
- Scans are throttled by `NormalTargetingDelay` and `GuardAreaTargetingDelay` for every techno alike.

## Where it hooks in OpenYR

What exists today: `TechnoClass::What_Weapon_Should_I_Use` in `code/techno.cpp` already follows stock `SelectWeapon` rule by rule (turret weapons, gattling stages, open-topped, airstrike, drain, overpowered buildings, locomotor, `NavalTargeting`, `LandTargeting`). `TechnoClass::Naval_Weapon` is the stock `NavalTargeting` table and needs no change; the new code calls it. The type reads `WeaponCount` and `Weapon<n>` only when `Has_Multiple_Turrets()` is true (`TechnoTypeClass::Read_INI` in `code/techtype.cpp`; `WEAPON_SLOT_COUNT` is 18). `Evaluate_Object`, `Evaluate_Cell`, `Greatest_Threat_Scan` and `Target_Threat` in `code/techno.cpp` carry the candidate tests, the ring scan and the score. `TechnoClass::Can_Fire` has the fire checks and already refuses a berzerk shooter at a `BerserkFriendly` target. The engine reads none of the keys in this spec. It does not implement Ares `NoAmmoWeapon` or `NoAmmoAmount`, EMP status (so `ForceWeapon.UnderEMP` has nothing to test), or attack-move (`MISSION_ATTACK_MOVE` is unused).

Per-key mapping:

| Feature | OpenYR location |
|---|---|
| Weapon choice: force, multi, no-ammo, no-fallback | `TechnoClass::What_Weapon_Should_I_Use`. Insert the ammo rule and `ForceWeapon`/`MultiWeapon` after the open-topped test and before the airstrike, drain and primary/secondary rules. Put the no-fallback return at the start of the primary/secondary section (after `first`/`second` null and `IsNeverUse`). |
| Cell target and walls | The cell branch of `What_Weapon_Should_I_Use`; the wall paths in `TechnoClass::What_Action(Cell ...)`, `TechnoClass::Evaluate_Just_Cell`, and the wall checks in `InfantryClass` and `UnitClass` cell-entry code, all of which read weapon 0 today. |
| Berzerk house limit | `TechnoClass::Can_Fire`, beside the `IsBerserkFriendly` test. Phobos uses the exit its code calls `CannotFire`; `FIRE_CANT` (the `CANT_FIRE` label) is the likely match. |
| Non-threatening structures | The "building that cannot shoot back" block in `TechnoClass::Evaluate_Object`. It already uses `Is_Human_Player()`. |
| Zone test | `Greatest_Threat_Scan` (where `zone` is set), `Evaluate_Cell` and `Evaluate_Object`. `any` can set `zone` to -1. |
| Early ring returns | The two `radius == crange/4` and `radius == crange/2` returns in `Greatest_Threat_Scan`. |
| Extra threat | `TechnoClass::Target_Threat`, before `return(threat + 100000.0)`. The function already computes the target's weapon index against the scanner (`target_weapon_index`), which `IsThreat` can reuse. The last-target position lives on the scanner and updates in `TechnoClass::AI`. |
| Directions | `DirType` here is 8-bit. Multiply an angle difference by 256 so the facing coefficient means what it does in Phobos INI files. |
| Scan pacing | OpenYR has no per-object scan timer. `FootClass::Do_MISSION_GUARD`, `Do_MISSION_GUARD_AREA` and `Do_MISSION_HUNT` scan every mission `Rate` (`MissionControlClass::Normal_Delay`) plus `Random_Pick`. `Rule->NormalTargetingDelay` is read in `TechnoClass::AI` for `OpportunityFire` and `Rule->GuardAreaTargetingDelay` in `BuildingClass` guard code. |
| Multi weapon plumbing | `Combat_Damage`, `FootClass`/`UnitClass`/`InfantryClass::Greatest_Threat` (threat flags), `GuardRange`/`Threat_Range`, FLH reads in `TechnoTypeClass::Read_INI`, infantry firing sequences and `UnitClass` sync frames (for `IsSecondary`). All use `PrimaryWeapon` and `SecondaryWeapon` today. |
| Type, rules and weapon reads | `TechnoTypeClass::Read_INI`, `RulesClass::General` and `RulesClass::Combat_Damage` in `code/rules.cpp`, `WeaponTypeClass::Read_INI` in `code/weapon.cpp`. |

Build order, smallest first:

1. Read all keys and add them to the three `Serialize` methods. Run `python manual/tools/manage.py update`, document the keys on the owning manual page (`manual/content/systems/target-selection.md`) and raise `SaveVersionInfo::REVISION`.
2. `BerzerkTargeting`.
3. `AutoTarget.NoThreatBuildings`, `AutoTargetAI.NoThreatBuildings` and `AttackNoThreatBuildings`.
4. `DisableOveroptimizationInTargeting` and `TargetZoneScanType=any`. Settle Open questions 1 before releasing either.
5. `NoSecondaryWeaponFallback`, `.AllowAA` and `AllowWeaponSelectAgainstWalls` (with the wall paths).
6. Non-range `ForceWeapon.*` and `ForceAAWeapon.*`.
7. `ForceWeapon.InRange` and `ForceAAWeapon.InRange`, once Open questions 4 is decided.
8. Ares `NoAmmoWeapon`, `NoAmmoAmount`, then `NoAmmoWeapons`.
9. `ExtraThreat.*` with last-target tracking.
10. `TargetZoneScanType=inrange`.
11. `MultiWeapon` with its plumbing.
12. Scan pacing keys, after the owner decides how pacing should work (Open questions 6).

## Saved state

- Type, rules and weapon fields (all keys in Keys): parsed values that go through `TechnoTypeClass::Serialize`, `RulesClass::Serialize` and `WeaponTypeClass::Serialize`, as `WeaponCount` and `GuardAreaTargetingDelay` do now. The save revision must rise.
- Per techno, for `ExtraThreatCoefficient.DistanceToLastTarget`: the last target position (a coordinate) and the 45-frame clear timer. Serialize both in `TechnoClass::Serialize`. Add both to `TechnoClass::Compute_CRC`, because they change which target is chosen.
- Per techno, for `DistributeTargetingFrame`: the random 0 to 15 offset, or only the resulting timer value. Serialize it. Phobos draws it from the synchronized generator for every techno; drawing it only when the key is on keeps existing recordings valid (Open questions 6).
- If scan pacing keys are implemented with a per-object next-scan frame, that frame is per-techno saved and CRC state.
- No per-house or global runtime state. `IsBerzerk` and `BerzerkDuration` are saved already.

## Open questions

1. Does a real YR ring scan keep the highest score? OpenYR's `Greatest_Threat_Scan` never updates `bestval` in the ring loops, so each ground candidate replaces the previous one and the scan returns the last candidate it reached; `manual/content/systems/target-selection.md` documents this as stock behaviour. The Phobos docs for `DisableOveroptimizationInTargeting` and for `ExtraThreat` assume the highest score wins. If OpenYR is right, `ExtraThreat.*` cannot rank ground candidates on Guard, Area Guard, Move and Patrol scans (they work on whole-map scans and on the flying-object passes), and disabling the early returns makes the scan prefer the outermost candidate. Test 1 in the test plan settles this with the real game before steps 4 and 9 are built.
2. In which order do `ForceWeapon.*` and the special cases at the top of the stock function run? The Phobos hooks place the force step after the ammo rule. The docs do not say whether `ForceWeapon.*` beats `OpenTransportWeapon`, an overpowered building or `ElectricAssault`. OpenYR needs an owner decision; the safe choice is to run the force step after those special cases.
3. `NoSecondaryWeaponFallback` also blocks `NavalTargeting`, `LandTargeting` and the 0% `Verses` swap on two-weapon types, as far as the code shows. With `MultiWeapon` it only removes the secondary priority cases, and the in-order fallback to a later weapon still happens. The docs describe neither. Decide whether to copy this or to give the key one meaning.
4. `ForceWeapon.InRange` with no `Overrides` and `ApplyRangeModifiers=no` leaves the compared range at 0 in the Phobos code, so a position only matches at distance 0. The docs say the weapon's own `Range` applies. Implement the documented behaviour, and confirm with real Phobos before relying on it.
5. `NoAmmoWeapons` in Phobos is written `MultiWeaponCanFire(a, b, c), ignoreNeverUse` inside the `if`, with the closing parenthesis misplaced. The condition therefore reduces to `ignoreNeverUse`: with the default `yes` the first listed index is always taken, whether or not it can fire; with `no` the list is never used. Implement the documented first-usable behaviour. The code also tests `Ammo >= 0` where `CanFireNoAmmoWeapon` tests `Ammo > 0`.
6. Scan pacing. OpenYR scans are paced by the mission `Rate`, not by `NormalTargetingDelay`, so the six delay keys have no obvious place. Options: add a per-object next-scan frame that gates the Guard, Area Guard and Hunt scans only when a key is set, or drop these keys. `DistributeTargetingFrame` draws from the synchronized random generator for every techno in Phobos; copying that shifts every later random number. The attack-move pair has no mission to attach to.
7. `SelectMultiWeapon` prefers secondary weapons against air targets when `AllowAA=no` (it tests `!AllowAA`), and stops preferring them when `AllowAA=yes`. The two-weapon path treats `AllowAA` the other way round. This looks inverted.
8. `ExtraThreatCoefficient.Facing` in the docs reads as the target's own facing. The code uses the bearing from the scanner to the candidate. The docs give 15 frames of last-target memory; the code keeps 45.
9. `TargetZoneScanType=inrange` tests a cell near the candidate, not the scanner's position, against the weapon range. A scanner far from the candidate passes if any usable cell near the candidate is in range of it.
10. `BerzerkTargeting` leaves candidate selection alone. A berzerk techno can sit on an ally it refuses to fire at. Decide whether to also filter candidates in `Evaluate_Object` and `Evaluate_Cell`.
11. `Naval.Decloaked` tests `Cloakable` and `Naval`, not `Underwater`. A submarine type that is `Underwater=yes` without `Naval=yes` never matches.
12. `ExtraThreat_Enabled` is computed in Phobos while the type is read, using the `[General]` values at that moment. A later INI layer that changes the `[General]` defaults does not refresh it. OpenYR can test the effective values at use time.
13. `MaxGuardRange` and `AreaGuardRange` sit in the same Phobos section as the delays and change scan radius for `MultiWeapon` types. They are not specified here.

## Test plan

Use a skirmish or single-player test map with the editor. Define new weapons by copying an existing ground weapon section together with its projectile and warhead, so the test needs no new art. Call the copies `TestA` (Damage=1) and `TestB` (Damage=500). Both warheads have `Verses=100%` against every armor. Targets are stationary enemy units with `Strength=100`, so a `TestB` hit kills and a `TestA` hit does not. Put `Weapon<n>FLH=0,0,0` lines in art for any type that uses `Weapon<n>`.

1. Baseline ring scan (real Yuri's Revenge, without Phobos). Attacker: a vehicle with `GuardRange=10`, `TargetSpecialThreatCoefficient=100`, weapon range 8. Targets: infantry `X` with `SpecialThreatValue=10` placed exactly 3 cells east of the attacker, and infantry `Y` with `SpecialThreatValue=0` exactly 4 cells north. Put the attacker on Guard. Expected: `Y` is shot first if the scan returns the last candidate (OpenYR's reading); `X` is shot first if it keeps the highest score. Record which. This decides Open questions 1.
2. `DisableOveroptimizationInTargeting`. Same attacker. `X` (value 0) 3 cells east, `Z` (`SpecialThreatValue=10`) 7 cells north, weapon range 9. With the key `no`, `X` is shot first (the scan stops at the half ring, 5 cells). With `yes`, `Z` is shot first.
3. `ForceWeapon.Infantry` against stock. Attacker: `Primary=TestA`, `Secondary=TestB`, `ForceWeapon.Infantry=1`. Order an attack on an enemy infantry unit: it dies at the first hit. Remove the key and repeat: the infantry survives several volleys. Edge: order an attack on an enemy vehicle with the key set: the vehicle takes `TestA` damage only.
4. `ForceWeapon.Defenses` and `Buildings`. Same attacker with `ForceWeapon.Buildings=0`, `ForceWeapon.Defenses=1`. Attack an enemy power plant: it survives the first volley (`TestA`). Attack a Pillbox-class building (`BuildCat=Combat`): it dies at the first hit. Edge: set `ForceWeapon.Defenses=1` and remove `ForceWeapon.Buildings`: a power plant is hit by the stock-chosen weapon (`TestA`).
5. `ForceWeapon.InRange`. Attacker: `Primary=TestA` with `Range=8`, `Secondary=TestB` with `Range=4`, `ForceWeapon.InRange=1` and `ForceWeapon.InRange.Overrides=4`. Target a stationary vehicle 3 cells away: it dies at the first hit. Target one 6 cells away: the unit fires `TestA` from where it stands and the target survives. Edge: remove `Overrides` and repeat the 3-cell case. By the docs the first hit kills (the weapon's own range of 4). Record what happens, since the Phobos code may never force the weapon (Open questions 4).
6. `NoSecondaryWeaponFallback`. Attacker: `Primary=TestA` with a projectile `AA=no`, `Secondary=TestAA` with `AA=yes` (copy an AA weapon). Without the key, the unit fires at a hovering enemy aircraft with `TestAA`. With `NoSecondaryWeaponFallback=yes` the unit does not fire. With `NoSecondaryWeaponFallback.AllowAA=yes` added, it fires `TestAA` again. Edge: use `Secondary=TestB` (a ground weapon) and give `TestA`'s warhead `Verses=0%` against `heavy` armor. Order an attack on a heavy-armor vehicle. With the key on it takes no damage and the unit does not switch to `TestB`; without the key it switches and the vehicle takes `TestB` damage.
7. `MultiWeapon`. A vehicle with `MultiWeapon=yes`, `WeaponCount=3`, `Weapon1=TestA` (warhead `Verses=0%` against `concrete`, and the test building has `Armor=concrete`), `Weapon2=TestAA`, `Weapon3=TestB`. Attack an enemy building: it dies at the first hit (index 2 is the first weapon that can fire). Attack infantry: only `TestA` damage. Attack an aircraft: `TestAA` fires. Edge: add `MultiWeapon.SelectCount=2` and attack the building again: the unit does not fire. Then add `ForceWeapon.Buildings=2`: it fires `TestB`.
8. `NoAmmoWeapons`. Needs the Ares ammo keys. A `MultiWeapon` vehicle with `Ammo=1`, `NoAmmoAmount=0`, `NoAmmoWeapon=1`, `NoAmmoWeapons=1,2`, `Weapon2=TestAA`, `Weapon3=TestA`, `Weapon1=TestB`. Attack a ground unit. The first shot is `TestB` and kills. After the ammo is spent, attack a second ground unit. By the docs the unit fires `TestA`, because index 1 cannot hit ground. Phobos' code may fire nothing (Open questions 5).
9. `BerzerkTargeting`. In the map, a trigger action `Go Berzerk` on a rifle infantry unit with two other infantry units next to it: one ally, one enemy. Put `BerzerkTargeting=enemies` under `[CombatDamage]`. Expected: the ally takes no damage and the enemy dies. With `none`, nobody takes damage. With the default `all`, the nearest unit is attacked, ally or not. Edge: remove the enemy and keep the ally: the berzerk unit stands still. Move the key to `[General]` and repeat to see which section the engine honours.
10. `AutoTarget.NoThreatBuildings` and `AttackNoThreatBuildings`. A player-owned infantry on Guard 2 cells from an enemy Power Plant, nothing else in range. Stock: it does not fire. With `AutoTarget.NoThreatBuildings=yes` under `[General]`: it fires at the plant. With the global key `yes` and `AttackNoThreatBuildings=no` on the infantry's weapon: it does not fire. With the global key `no` and the weapon key `yes`: it fires.
11. `ExtraThreat`. Whole-map scan (put the attacker on Hunt through a trigger). Two enemy units at equal distance and equal type, one in front of the turret and one behind. Stock: the first in scan order is chosen. With `ExtraThreatCoefficient.Facing=-0.01`: the one in front is chosen. With `ExtraThreat.IsThreat=100` and an armed target plus an unarmed one of the same type: the armed one is chosen. With `AlwaysConsideredThreat=yes` on the unarmed one and `IsThreat=100` on the attacker: both get the bonus and the choice returns to scan order.
12. `TargetZoneScanType`. An artillery attacker with range 14 on one side of a river 4 cells wide that ground units cannot cross, with an enemy unit on the other bank within 12 cells. Put it on Hunt. `same`: it does nothing. `any`: it fires. `inrange`: it fires. Move the enemy 20 cells from the bank: `any` targets it and `inrange` does not.
13. Targeting delay. A guard unit with `PlayerNormalTargetingDelay=300`, an enemy walking into range just after the unit's last empty scan. The first shot comes at least 300 frames later than with the key unset, if the pacing is implemented (Open questions 6). Record the delay with the in-game frame counter.
