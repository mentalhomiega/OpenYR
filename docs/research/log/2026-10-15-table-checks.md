# Research log, 2026-10-15: table checks

This session compared 632 rows of the two tables with the Phobos and Ares documentation and changed 89 of them (84 Phobos, 5 Ares). One Phobos row was removed in a merge. All 14 Phobos rows ticked on 2026-10-09 stay ticked. Nothing under `code/` changed, and the name lists in `tools/data/` did not change because no tag was added or renamed.

## Rows checked

The rows come from the items ranked 1 to 7 in [`priorities.md`](../priorities.md), with more weight on Phobos. For each row the effect sentence, section and default were compared with the documentation page the row links. Sizes and dependency columns were read for plausibility and not re-estimated.

| Group | Rows |
|---|---|
| Phobos shields (all `ShieldType`, `ShieldTypes`, `Pips.*` and `Shield.*` rows) | 104 |
| Phobos attached effects (all rows) | 149 |
| Phobos weapon selection and targeting (`ForceWeapon.*`, `NoSecondaryWeaponFallback`, `MultiWeapon`, `NoAmmoWeapons`, `ExtraThreat*`, targeting delays, guard ranges, `TargetZoneScanType`) | 59 |
| Phobos per-type copies of global rules (`CurleyShuffle`, `Wake*`, `Harvester*Rate`, `Occupy*`, `Bunker*`, `Units.Repair*`) | 29 |
| Phobos weapons, small switches (`LaserPositionUpdate` through `Anim.Update`) | 97 |
| Phobos warheads, small switches (merge, mind control, `Crit.*`, damage multipliers, Ivan bombs, `KillWeapon`, `TransactMoney`, crew escape) | 77 |
| Phobos ticked rows from 2026-10-09, plus `FreeMCV.CreditsThreshold` | 15 |
| Phobos `AnimRemapDefaultColorScheme` (both rows) | 2 |
| Ares `Versus.<armor>` and its three variants | 4 |
| Ares veterancy, experience, culling, bounty, crush damage, survivors, self healing, academies and spy effects | 96 |

The Phobos `ShieldType`, `AttachEffectType`, targeting, per-type and several warhead rows were also read against the Phobos source for key spelling, defaults and units. The other Phobos rows and all Ares rows were compared with the documentation only, because Ares has no readable source.

## The 14 ticked rows

All 14 keys are read by the engine from the section the row names. The manual has a page for each key.

| Row | Section | Engine evidence |
|---|---|---|
| `FreeMCV` | `[CrateRules]` | `code/rules.cpp` reads it into `IsFreeMCV`; nothing else uses the member, and `manual/content/keys/freemcv.md` says it has no effect |
| `EnemyInsignia` | `[General]` | `code/rules.cpp` reads it; `code/techno.cpp` uses it when drawing insignia |
| `Insignia`, `InsigniaFrame`, `InsigniaFrames`, `Insignia.ShowEnemy` | TechnoType | `code/techtype.cpp` reads all four; `code/techno.cpp` draws with them |
| `HealthBar.Hide` | TechnoType | `code/techtype.cpp` reads it; `code/techno.cpp` skips the health bar when it is set |
| `OpenTopped.RangeBonus`, `OpenTopped.DamageMultiplier`, `OpenTopped.WarpDistance` | TechnoType | `code/techtype.cpp` reads them; `code/techno.cpp` and `code/temporal.cpp` apply them |
| `OpenTransport.RangeBonus`, `OpenTransport.DamageMultiplier` | TechnoType and `[CombatDamage]` | `code/techtype.cpp` and `code/rules.cpp` read them; `code/techno.cpp` applies them |

`FreeMCV` keeps its tick because the engine reads the key, but the row now says the engine ignores it. Whether a tick should also require an effect is open (see the end of this log).

## Merged row

The two `AnimRemapDefaultColorScheme` rows are now one row in Global settings. It covers an `AltPalette=yes` animation with no owner and a `Voxel=yes` projectile image that cannot remap, and links both documentation pages. The Phobos source also uses the scheme for a bomb parachute animation that has no owner, and the row says so. The copy in the Animations group is gone.

## Corrections

"Change" is `fixed` where the old text was wrong or unsupported, and `completed` where it was right but lacked a default, unit or condition.

### Phobos

| Tag | Section | Change | Reason |
|---|---|---|---|
| `Shield.ConditionYellow` | [AudioVisual] | fixed | Thresholds are fractions of Strength in the source; the docs' "absolute" form is not read. |
| `Shield.ConditionRed` | [AudioVisual] | fixed | Same as Shield.ConditionYellow. |
| `ConditionYellow` | ShieldType | fixed | Same as Shield.ConditionYellow. |
| `ConditionRed` | ShieldType | fixed | Same as Shield.ConditionYellow. |
| `Powered` | ShieldType | fixed | Low power takes only a building offline; deactivation and EMP take any owner offline. |
| `SelfHealing.EnabledBy` | ShieldType | fixed | The listed building must also be active (not deactivated or under EMP, with power). |
| `SelfHealing.Rate` | ShieldType | completed | 0 applies a step on every game frame. |
| `Respawn.Rate` | ShieldType | completed | 0 respawns on the next game frame. |
| `Powered` | AttachEffectType | fixed | Low power applies only to buildings, as for shields. |
| `OpenTransport.RangeBonus` | [CombatDamage] | completed | Default added (0). |
| `OpenTransport.DamageMultiplier` | [CombatDamage] | completed | Default added (1.0). |
| `OpenTopped.RangeBonus` | TechnoType | completed | Default [CombatDamage] OpenToppedRangeBonus added. |
| `OpenTopped.DamageMultiplier` | TechnoType | completed | Default [CombatDamage] OpenToppedDamageMultiplier added. |
| `OpenTopped.WarpDistance` | TechnoType | fixed | The key sets the distance at which a passenger's temporal weapon releases its target. |
| `OpenTransport.RangeBonus` | TechnoType | completed | Adds to the transport's bonus; default [CombatDamage] value added. |
| `OpenTransport.DamageMultiplier` | TechnoType | completed | Multiplies with the transport's multiplier; default added. |
| `FreeMCV` | [CrateRules] | completed | The engine reads the key but ignores it; the row says so. Tick kept. |
| `FreeMCV.CreditsThreshold` | [CrateRules] | completed | Default 1500 added. |
| `InsigniaFrames` | TechnoType | completed | Phobos gives InsigniaFrame priority over this list; the engine lets the list override a plain InsigniaFrame. |
| `NoSecondaryWeaponFallback` | TechnoType | completed | Exemptions added (WeaponN units and fixed-Secondary cases). |
| `AllowWeaponSelectAgainstWalls` | TechnoType | completed | Wall=yes condition and NoSecondaryWeaponFallback interaction added. |
| `AllowWeaponSelectAgainstWalls` | [CombatDamage] | completed | Default false added. |
| `AlwaysConsideredThreat` | TechnoType | fixed | It only feeds the ExtraThreat.IsThreat bonus. |
| `ExtraThreatCoefficient.Facing` | TechnoType | completed | Unit added (full circle 65536). |
| `ForceWeapon.InRange.TechnoOnly` | [General] | completed | Default true added. |
| `ForceWeapon.InRange.TechnoOnly` | TechnoType | completed | Default [General] value added; "ground" clarified as empty ground. |
| `TargetZoneScanType` | TechnoType | completed | Default same, and the aircraft exclusion, added. |
| `MaxGuardRange` | TechnoType | completed | Default 16 cells added. |
| `NoAmmoWeapons` | TechnoType | fixed | Source quirk added: the selection test ends in a comma expression, so IgnoreNeverUse alone decides. |
| `NoAmmoWeapons.IgnoreNeverUse` | TechnoType | fixed | Meaning of yes stated (NeverUse weapons stay eligible). |
| `CurleyShuffle` | AircraftType | completed | Stock meaning and [General] default added. |
| `Wake` | TechnoType | completed | Default [General] Wake added. |
| `Wake.Grapple` | TechnoType | completed | Default Wake added. |
| `Wake.Sinking` | TechnoType | completed | Default Wake added. |
| `HarvesterDumpRate` | VehicleType | completed | Unit (game minutes) and [General] default added. |
| `HarvesterLoadRate` | VehicleType | completed | Unit (frames per step) and [General] default added. |
| `OccupyDamageMultiplier` | BuildingType | completed | Default [CombatDamage] value added. |
| `OccupyROFMultiplier` | BuildingType | completed | Default [CombatDamage] value added. |
| `BunkerDamageMultiplier` | BuildingType | completed | Documented default added; one source hook falls back to OccupyDamageMultiplier. |
| `BunkerROFMultMultiplier` | BuildingType | completed | Source fallback [CombatDamage] BunkerROFMultiplier added. |
| `Units.RepairRate` | BuildingType | fixed | The value is in game minutes (the row said frames); defaults per building kind added. |
| `Units.RepairStep` | BuildingType | completed | Default [General] RepairStep added. |
| `Units.RepairPercent` | BuildingType | completed | Default [General] RepairPercent and the 1-credit floor added. |
| `Units.UseRepairCost` | BuildingType | completed | Default (false for infantry, true otherwise) added. |
| `CanTarget.IronCurtained` | [CombatDamage] | completed | Default true added; the "per-weapon override" wording moved to the weapon row. |
| `CanTargetAI.IronCurtained` | [CombatDamage] | completed | Default false added. |
| `AutoTarget.IronCurtained` | [CombatDamage] | fixed | "Per-weapon/projectile key" corrected to weapon only; default true added. |
| `CanTarget.IronCurtained` | WeaponType | fixed | Row repeated the global text; the per-weapon default now depends on the player type. |
| `AutoTarget.IronCurtained` | WeaponType | completed | Computer players ignore it; default added. |
| `ExtraRange.TargetMoving`, `ExtraRange.TargetMoving.CloseRangeOnly`, `ExtraRange.FirerMoving`, `ExtraRange.Prefiring`, `ExtraRange.Prefiring.IncludeBurst` | [General] | fixed | "Per-weapon/projectile key" corrected to per-weapon; no BulletType key exists. |
| `OmniFire.TurnToTarget`, `CylinderRangefinding`, `Strafing.SimulateBurst`, `Strafing.UseAmmoPerShot`, `Strafing.TargetCell` | [General] | fixed | Same correction. |
| `ROF.RandomDelay` | [CombatDamage] | fixed | Same correction. |
| `EBoltZAdjust.ClampInitialDepthForBuilding` | [AudioVisual] | fixed | Same correction. |
| `VisualScatter.Min`, `VisualScatter.Max` | [AudioVisual] | fixed | They are global values with defaults 0.03 and 0.13; no per-weapon Min/Max key exists. |
| `VisualScatter` | WeaponType | completed | Boolean semantics and default false added. |
| `Strafing.SimulateBurst` | WeaponType | completed | Applies only with Burst=1 or no Burst. |
| `Strafing.EndDelay` | WeaponType | completed | Default formula added. |
| `RemoveMindControl.Silent` | WarheadType | fixed | It mutes the MindClearedSound; the row also claimed the release animation. |
| `MindControl.Permanent.ReplaceSilent` | [AudioVisual] | fixed | Global only, used when MindControl.Permanent replaces a conventional mind control; the row said "permanent mind controller" and a warhead section. |
| `VeteranCritChance` | TechnoType / [General] | fixed | Section was WarheadType; it multiplies crit chance for the CRITCHANCE ability (the row said it adds to it). |
| `Crit.AnimOnAffectedTargets` | WarheadType | fixed | Plays Crit.AnimList on each affected target in addition to the normal AnimList. |
| `Crit.AnimList` | WarheadType | completed | Replaces AnimList when a crit affects at least one target. |
| `Crit.ActiveChanceAnims` | WarheadType | fixed | Shown at the detonation point whenever the warhead has a crit chance (the row said the choice depends on the chance value). |
| `ImmuneToCrit` | TechnoType | fixed | Section was WarheadType. |
| `DamageOwnerMultiplier.NotAffectsEnemies`, `DamageAlliesMultiplier.NotAffectsEnemies` | [CombatDamage] | fixed | "Unclear" replaced: defaults for warheads with AffectsEnemies=false; read from [CombatDamage] only. |
| `DamageOwnerMultiplier.Berzerk`, `DamageAlliesMultiplier.Berzerk`, `DamageEnemiesMultiplier.Berzerk` | WarheadType | fixed | Applies when the firer is berserk (the row said the target). |
| `DamageSourceHealthMultiplier`, `DamageTargetHealthMultiplier` | WarheadType | fixed | The value times the health fraction is added to the damage multiplier (the row said the damage is scaled by the fraction). |
| `IvanBomb.Detonate.AffectsParasite` | WarheadType | fixed | Applies to bombs planted on a parasite inside the victim (the row said units carrying a parasite). |
| `SuppressKillWeapons`, `SuppressKillWeapons.Types` | TechnoType | fixed | Section was WarheadType; the key stops this techno's death from triggering kill weapons. |
| `AnimRemapDefaultColorScheme` | [AudioVisual] | fixed | Two rows merged into one covering AltPalette animations and Voxel=yes projectile images; the Animations copy removed. |

### Ares

| Tag | Section | Change | Reason |
|---|---|---|---|
| `Versus.<armor>` | WarheadType | completed | Default when unset added. |
| `Versus.<armor>.ForceFire`, `.Retaliate`, `.PassiveAcquire` | WarheadType | completed | Default from the 0%, 1% and 2% special values added. |
| `Promote.EliteFlash` | TechnoType | completed | Depends-on column now names EliteFlashTimer, as the Veteran row does. |

## Not decided

- Tick meaning: `FreeMCV` is read and ignored. If a tick should mean "has an effect", `FreeMCV` loses it.
- `InsigniaFrames`: the engine lets the list override a plain `InsigniaFrame`; Phobos does the opposite. Both orders agree when only one of the keys is set.
- Shield conditions: the Phobos documentation says `ConditionYellow` and `ConditionRed` accept absolute values; the source compares fractions only. A test in the real game would settle which one the shipped Phobos follows.
- Phobos source quirks recorded in rows, not specified as intended behaviour: the `NoAmmoWeapons` selection test, and the `BunkerDamageMultiplier` fallback to `OccupyDamageMultiplier` in one hook.
- Ares `Versus.<armor>.ForceFire`, `.Retaliate` and `.PassiveAcquire` defaults come from the pages on the 0%, 1% and 2% special values. The pages do not state the default of an unset key in so many words.
- Duplicated Ares rows: `CrushDamage.*`, `Bounty.*Value`, `Survivor.*Chance`, `SelfHealing.*Max` and `*Amount`, `Missile.Elite*` and `Insignia.*` each appear as one combined row and again as separate rows. The copies agree. Merging them is left to a later session.
- Not in the tables: the Phobos veteran and elite ability names `CRITCHANCE` and `CRITIMMUNE`, and the `Tint.Cumulative` key that the source also reads for a `ShieldType`. They are values or undocumented keys, so the name lists stay unchanged.
- Attached-effect `DurationOverrides` entries of 0 keep the effect type's `Duration` in the source. The four rows that carry the key do not say so.
- ModEnc was not consulted. The `CurleyShuffle` meaning comes from the manual page in this repository.

## Checks

- `python3 -m unittest discover -s docs/research/tools/tests`: 11 tests passed.
- A script confirmed that every row in both tables has 7 columns, every `code/` file named exists, and no tag repeats within a section (`phobos-tags.md` now has 2,204 rows, `ares-tags.md` 1,097).
- `tools/data/ares.txt` and `tools/data/phobos.txt` were not rebuilt.

## Next session

1. Check the remaining Phobos rows (about 1,670) against the documentation, starting with the Technos and Interface groups, which have the most rows and the fewest checks.
2. Check the Ares Technos, Buildings and Superweapons rows that are not about veterancy.
3. Carry on with the specs listed at the end of [`2026-10-09.md`](2026-10-09.md).
