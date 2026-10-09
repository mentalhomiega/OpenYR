# Shields (Phobos)

Source: Phobos `docs/New-or-Enhanced-Logics.md` section "Shields"; `src/New/Type/ShieldTypeClass.*`, `src/New/Entity/ShieldClass.*`, `src/Ext/Techno/Hooks.ReceiveDamage.cpp`, `Hooks.cpp`, `Hooks.Firing.cpp`, `Hooks.TargetEvaluation.cpp`, `Hooks.Pips.cpp`, `Body.Update.cpp`, `src/Ext/WarheadType/Detonate.cpp`, `Body.cpp`, `src/Ext/Bullet/Hooks.cpp`, `src/Ext/Foot/Body.cpp`, `src/Ext/Unit/Hooks.DeploysInto.cpp`, `src/Ext/TEvent/Body.cpp`.

A shield is a second hit-point pool on a techno. It has its own maximum, its own armor, and optional self-healing and respawn. Damage goes to the shield first. This page uses "shield" for this feature only. The Force Shield superweapon is a different feature and is called "Force Shield".

## Keys

### rules.ini: registry and TechnoType

| Key | Section | Type | Default |
|---|---|---|---|
| `0=`, `1=`, ... | `[ShieldTypes]` | ShieldType section names (the key names do not matter, the values do) | empty list |
| `ShieldType` | any TechnoType | ShieldType name | none |

`ShieldType=` creates the type if the name is new, so a typo gives a type with default values (`Strength=0`, no shield) and no error. Warhead keys that list shield types (`Shield.AttachTypes` and the others below) only accept names already registered in `[ShieldTypes]` and report a parse error otherwise. The name `none` is reserved.

### rules.ini: global

| Key | Section | Type | Default |
|---|---|---|---|
| `ShieldApplyArmorMult` | `[CombatDamage]` | boolean | `no` |
| `Shield.ConditionYellow` | `[AudioVisual]` | float | the game's `ConditionYellow` |
| `Shield.ConditionRed` | `[AudioVisual]` | float | the game's `ConditionRed` |
| `Pips.Shield` | `[AudioVisual]` | three integers (pips.shp frames, zero-based: green, yellow, red) | `-1,-1,-1` |
| `Pips.Shield.Building` | `[AudioVisual]` | three integers | `-1,-1,-1` |
| `Pips.Shield.Background` | `[AudioVisual]` | SHP file name | `pipbrd.shp` |
| `Pips.Shield.Building.Empty` | `[AudioVisual]` | integer frame | `0` |

### rules.ini: ShieldType section

Each section named in `[ShieldTypes]` is read from rules.ini. Rates are in game minutes of 900 frames and are truncated to whole frames when read. A section that does not exist leaves every key at its default.

| Key | Type | Default |
|---|---|---|
| `Strength` | integer | `0` (a type with `Strength` of 0 or less gives no shield) |
| `InitialStrength` | integer | `Strength`; a larger value is capped to `Strength` |
| `ConditionYellow`, `ConditionRed` | float, a fraction of `Strength` (`50%` or `0.5`) | `[AudioVisual] Shield.ConditionYellow` / `Shield.ConditionRed`, then the game's `ConditionYellow` / `ConditionRed` |
| `Armor` | ArmorType | `none` |
| `InheritArmorFromTechno` | boolean | `no` |
| `InheritArmor.Allowed` | list of TechnoTypes | empty |
| `InheritArmor.Disallowed` | list of TechnoTypes | empty |
| `ApplyArmorMult` | boolean | `[CombatDamage] ShieldApplyArmorMult` |
| `Powered` | boolean | `no` |
| `AbsorbOverDamage` | boolean | `no` |
| `AbsorbPercent` | float | `1.0` |
| `PassPercent` | float | `0.0` |
| `ReceivedDamage.Minimum` | integer | `-2147483648` |
| `ReceivedDamage.Maximum` | integer | `2147483647` |
| `SelfHealing` | float: 0 off, -1.0 to 1.0 a fraction of `Strength`, any other value whole points, negative drains | `0` |
| `SelfHealing.Rate` | float, game minutes | `0` (a step every frame) |
| `SelfHealing.RestartInCombat` | boolean | `yes` |
| `SelfHealing.RestartInCombatDelay` | integer frames | `0` |
| `SelfHealing.EnabledBy` | list of BuildingTypes | empty (no restriction) |
| `Respawn` | float, same units as `SelfHealing`, only positive values respawn | `0` (never respawns) |
| `Respawn.Rate` | float, game minutes | `0` (respawns on the next shield update) |
| `Respawn.RestartInCombat` | boolean | `yes` |
| `Respawn.RestartInCombatDelay` | integer frames | `0` |
| `Respawn.Anim` | list of AnimationTypes | empty |
| `Respawn.Weapon` | WeaponType | none |
| `BreakAnim`, `HitAnim` | list of AnimationTypes | empty |
| `BreakWeapon` | WeaponType | none |
| `HitFlash` | boolean | `no` |
| `HitFlash.FixedSize` | integer | unset (twice the shield damage) |
| `HitFlash.Red`, `HitFlash.Green`, `HitFlash.Blue` | boolean | `yes` |
| `HitFlash.Black` | boolean | `no` |
| `IdleAnim`, `IdleAnim.ConditionYellow`, `IdleAnim.ConditionRed` | AnimationType | none |
| `IdleAnimDamaged`, `IdleAnimDamaged.ConditionYellow`, `IdleAnimDamaged.ConditionRed` | AnimationType | none |
| `IdleAnim.OfflineAction`, `IdleAnim.TemporalAction` | `None`, `Hides`, `Temporal`, `Paused`, `PausedTemporal` | `Hides` |
| `BracketDelta` | integer pixels | `0` |
| `Pips` | three integers | `-1,-1,-1` (then `[AudioVisual] Pips.Shield`) |
| `Pips.Building` | three integers | `-1,-1,-1` (then `Pips.Shield.Building`) |
| `Pips.Background` | SHP file name | `[AudioVisual] Pips.Shield.Background` |
| `Pips.Building.Empty` | integer frame | `[AudioVisual] Pips.Shield.Building.Empty` |
| `Pips.HideIfNoStrength` | boolean | `no` |
| `AllowTransfer` | boolean | unset: `yes` for a shield attached by a warhead, `no` for the type's own `ShieldType` |
| `AllowTransfer.Convert` | boolean | `AllowTransfer` |
| `ImmuneToBerserk` | boolean | `no` |
| `ImmuneToCrit` | boolean | `no` |
| `Tint.Color` | R,G,B | `0,0,0` |
| `Tint.Intensity` | float | `0.0` |
| `Tint.VisibleToHouses` | `none`, `owner`/`self`, `allies`/`ally`, `team`, `enemies`/`enemy`, `neutral`, `all` (a list) | `all` |

The tint is active only when `Tint.Color` is not `0,0,0` or `Tint.Intensity` is not 0. The shared tint reader also parses `Tint.Cumulative`, which has no effect on shields.

### rules.ini: WarheadType

| Key | Type | Default |
|---|---|---|
| `Shield.Penetrate` | boolean | `no` |
| `Shield.Break` | boolean | `no` |
| `Shield.BreakAnim`, `Shield.HitAnim` | list of AnimationTypes | empty (use the ShieldType's list) |
| `Shield.SkipHitAnim` | boolean | `no` |
| `Shield.HitFlash` | boolean | `yes` |
| `Shield.BreakWeapon` | WeaponType | none (use the ShieldType's) |
| `Shield.AbsorbPercent`, `Shield.PassPercent` | float | unset (use the ShieldType's) |
| `Shield.ReceivedDamage.Minimum`, `Shield.ReceivedDamage.Maximum` | integer | unset (use the ShieldType's) |
| `Shield.ReceivedDamage.MinMultiplier`, `Shield.ReceivedDamage.MaxMultiplier` | float | `1.0` |
| `Shield.Respawn.Duration` | integer frames | `0` |
| `Shield.Respawn.Amount` | float | unset (use `Respawn`) |
| `Shield.Respawn.Rate` | float, game minutes | `-1.0` (any negative value uses the ShieldType's) |
| `Shield.Respawn.RestartInCombat` | boolean | unset (use the ShieldType's) |
| `Shield.Respawn.RestartInCombatDelay` | integer frames | `-1` (any negative value uses the ShieldType's) |
| `Shield.Respawn.RestartTimer` | boolean | `no` |
| `Shield.Respawn.Anim` | list of AnimationTypes | empty |
| `Shield.Respawn.Weapon` | WeaponType | none |
| `Shield.SelfHealing.Duration` | integer frames | `0` |
| `Shield.SelfHealing.Amount` | float | unset (use `SelfHealing`) |
| `Shield.SelfHealing.Rate` | float, game minutes | `-1.0` |
| `Shield.SelfHealing.RestartInCombat` | boolean | unset |
| `Shield.SelfHealing.RestartInCombatDelay` | integer frames | `-1` |
| `Shield.SelfHealing.RestartTimer` | boolean | `no` |
| `Shield.AffectTypes` | list of ShieldTypes | empty (all types) |
| `Shield.Penetrate.Types`, `Shield.Break.Types`, `Shield.Respawn.Types`, `Shield.SelfHealing.Types` | list of ShieldTypes | unset (use `Shield.AffectTypes`) |
| `Shield.AttachTypes`, `Shield.RemoveTypes` | list of ShieldTypes | empty |
| `Shield.RemoveAll` | boolean | `no` |
| `Shield.ReplaceOnly` | boolean | `no` |
| `Shield.ReplaceNonRespawning` | boolean | `no` |
| `Shield.MinimumReplaceDelay` | integer frames | `0` |
| `Shield.InheritStateOnReplace` | boolean | `no` |

### Map file

| Item | Meaning |
|---|---|
| Trigger event 600 "Shield of the attached object is broken" | Fires when the attached object takes a hit while its shield is broken or missing. |

### Where docs and code disagree

- `InheritArmor.Disallowed`: the docs say it applies when `InheritArmor.Allowed` is empty. The code ignores it in that case.
- `ConditionYellow` and `ConditionRed`: the docs say "percents or absolute". The code multiplies the value by `Strength`, so only fractions work.
- `Shield.Respawn.Amount` and `Shield.SelfHealing.Amount`: the docs say a value of 0 or less keeps the ShieldType value. The code keeps it only when the key is unset. An explicit 0 turns the feature off for the duration. A negative `Shield.SelfHealing.Amount` drains, and a negative `Shield.Respawn.Amount` never respawns.
- `Shield.AffectTypes`: the docs say it limits "any of the effects listed above". The code uses it only for `Shield.AbsorbPercent`, `Shield.PassPercent`, `Shield.Penetrate`, `Shield.Break`, and the respawn and self-healing modifiers.
- `Pips.Background`: the docs allow `.pcx`; the code reads an SHP value.

## Behaviour

### Creation and lifetime

- A techno gets a shield when it is created if its type has `ShieldType=` with `Strength` above 0. The shield starts at `InitialStrength`, or at `Strength` when unset.
- A warhead can attach a shield later (`Shield.AttachTypes`) and remove it (`Shield.RemoveTypes`, `Shield.RemoveAll`). Removing the shield also clears the techno's own `ShieldType` for the rest of its life, except through type conversion (below).
- The shield is deleted when the owner's health reaches 0, is no longer alive, or is sinking.
- The shield is updated once per frame together with the owner. The update does nothing while the owner is in limbo (for example inside a transport) or while Phobos' `IsImmobilized` flag is set on it (see open questions).

### Which armor counts

- While the shield is active and the warhead cannot penetrate it, the shield's armor replaces the owner's armor for damage and for every `Verses` check used for targeting. "Active" means strength above 0 and not offline (`Powered`, below).
- `InheritArmorFromTechno=yes` uses the owner's own armor instead of `Armor`, for owners allowed by `InheritArmor.Allowed`. An empty `Allowed` list allows all owners, and then `Disallowed` has no effect in the code.
- A warhead can penetrate the shield with `Shield.Penetrate=yes`, with a Psychedelic warhead (unless the ShieldType has `ImmuneToBerserk=yes`), or both. When `Shield.Penetrate.Types` or `Shield.AffectTypes` is non-empty and does not list this ShieldType, the warhead does not penetrate. A penetrating warhead is checked against the owner's armor and skips the shield.
- The armor swap covers: weapon fire permission, passive target choice, retaliation, threat rating, weapon choice between primary and secondary, gattling stage choice, shrapnel target choice, and the `Verses` test that decides whether warhead side effects apply (`EffectsRequireVerses`).

### Damage order

The shield step runs at the start of the owner's damage routine, before armor multipliers, Iron Curtain and the rest of the stock damage code. It is skipped for forced damage (`IgnoreDefenses`), so the C4 warhead kills and EMP threshold kills reach hit points directly. Phobos' own health, veterancy and invoker filters and its damage multipliers (`DamageEnemiesMultiplier` and similar) run before it, so the shield sees the multiplied damage.

For a hit on an owner whose shield is active:

1. The shield is skipped, and the whole hit goes to hit points, when: the damage is 0; the owner is under a temporal beam; the owner is Iron Curtained or Force Shielded and the warhead cannot penetrate that (`PenetratesIronCurtain`, `PenetratesForceShield`); the warhead penetrates the shield; the owner's type has `Immune=yes`; or the owner has `TypeImmune=yes` and the attacker is the same type from the same house.
2. If the warhead's house filter (`AffectsOwner`, `AffectsAllies`, `AffectsEnemies`) excludes the target, or the warhead is a Temporal warhead, the shield and the hit points both take 0.
3. Positive damage `D`:
   - With `ApplyArmorMult=yes`, `D` is first divided by the owner's armor multiplier (techno multiplier times house multiplier times attached-effect multipliers times the veteran `VeteranArmor` factor, truncated, never below 0). With `no` the multiplier is not applied to the shield.
   - The stock damage formula is applied with the shield's armor and the distance to the blast center (falloff, then `Verses`, capped by `MaxDamage`). Call the result `S`.
   - Shield damage is `S` times `AbsorbPercent` (truncated). Hit point damage is the unmodified `D` times `PassPercent` (truncated); it is not reduced by the shield's armor and still goes through the owner's armor later.
   - `AbsorbPercent` and `PassPercent` come from the warhead (`Shield.AbsorbPercent`, `Shield.PassPercent`) when set and when `Shield.AffectTypes` is empty or lists this ShieldType; otherwise from the ShieldType.
   - Shield damage is then clamped to the minimum and maximum. The minimum is the warhead's `Shield.ReceivedDamage.Minimum` or the ShieldType's, times `Shield.ReceivedDamage.MinMultiplier`; the maximum likewise. The multiplication is overflow-safe (`SafeMultiply`). The clamp does not check `Shield.AffectTypes`.
4. Shield damage above 0:
   - The owner's house gets the "base under attack" alert for a building, or the harvester alert for a harvester, only when the owner belongs to the local player and the warhead is not `Nonprovocative`. The alert exists because a fully absorbed hit never reaches the stock alert code.
   - The owner uncloaks when the warhead (or the global setting) has `DecloakDamagedTargets`.
   - If shield damage is at least the current shield strength, the shield breaks. With `AbsorbOverDamage=yes` only the `PassPercent` part reaches hit points. Otherwise the part reaching hit points is `(absorbed damage before clamping - old strength)` times the armor multiplier (1 when `ApplyArmorMult=no`) divided by the warhead's `Verses` against the shield armor, floored at 0, plus the `PassPercent` part. Dividing by `Verses` turns the leftover back into unmodified damage, which the stock code then applies against the owner's armor.
   - Otherwise the shield loses the shield damage. If the ShieldType has `HitFlash=yes` and the warhead has `Shield.HitFlash=yes`, a light flash is made at the owner (size `HitFlash.FixedSize`, or twice the shield damage). Unless `Shield.SkipHitAnim=yes`, one animation from the warhead's `Shield.HitAnim`, or the ShieldType's `HitAnim` when that is empty, plays on the owner. Only the `PassPercent` part reaches hit points.
5. Shield damage below 0 (healing): if the shield is full, the healing goes to the owner's hit points unchanged. Otherwise the shield gains the amount, capped at `Strength`, and the owner receives 0. A heal on a shielded owner that has a parasite eating it also ejects the parasite. Healing uses the same formula as damage, with the shield's armor and `AbsorbPercent`. `PassPercent` matters only when the shield part is 0, and then that share of the heal goes to hit points.
6. Shield damage of 0: only the `PassPercent` part reaches hit points.

After the shield step, if the shield consumed all the damage the stock "minimum 1 damage" rule is skipped. A fully absorbed hit still reaches the stock code with 0 damage, so retaliation, scatter and cloak-shimmer logic still run on the owner.

Trigger event 600 is raised for the owner on every damage call that the shield processed, and the event condition is true only when the shield is broken or missing at that moment.

Combat for the restart timers (below) is any hit with positive damage, whether or not the shield is still active, from a warhead that is not Psychedelic, and that either was reduced by the shield or has `Verses` against the owner's current armor times the damage of at least 1.

### Break

Breaking sets the strength to 0 and does the following:

- Starts the respawn timer if the effective `Respawn` (the warhead modifier if one is running, else the ShieldType) is non-zero, with the effective `Respawn.Rate`.
- Stops the self-healing timer and removes the idle animation.
- Plays one random animation from `Shield.BreakAnim` of the breaking warhead, or `BreakAnim` of the ShieldType when the warhead's list is empty. No animation plays while the owner is burrowed.
- Records the break frame (used by `Shield.MinimumReplaceDelay`) and refreshes the tint.
- Detonates the warhead's `Shield.BreakWeapon`, or the ShieldType's `BreakWeapon`, at the owner with the owner as firer.

A shield that drops to 0 or less through negative `SelfHealing` breaks the same way, using the ShieldType's break animation and weapon.

### Self-healing

- Runs while the shield is online, enabled (below) and above 0. The amount per step is read as: 0 does nothing; a value from -1.0 to 1.0 is that fraction of `Strength`, rounded; any other value is whole points, truncated. `1.0` is the full strength and `1.1` is one point.
- The timer interval is `SelfHealing.Rate` converted to frames. Every step adds the amount and restarts the timer. A positive result above `Strength` is capped and the timer stops. A result at or below 0 breaks the shield.
- With a rate of 0 (the default) a step happens on every update, so `SelfHealing=10` with no rate restores 10 points per frame.
- The timer starts when the shield takes damage that counts as combat (with `SelfHealing.RestartInCombat=yes`), or on the first update that finds the shield below `Strength`.
- `SelfHealing.RestartInCombat=yes` (default): damage that counts as combat restarts the timer from the full interval when the delay is 0. With a delay above 0 the timer stops and a separate delay timer runs; when it ends the interval timer starts from the full value. `no` ignores damage.
- A warhead can override rate, amount, `RestartInCombat` and delay for `Shield.SelfHealing.Duration` frames (`Shield.SelfHealing.*`). Another warhead with a duration replaces all four values and resets the duration. A negative rate or delay keeps the ShieldType value. If `Shield.SelfHealing.RestartTimer=yes` a running timer restarts, otherwise it is rescaled to the new rate and keeps its relative progress. When the duration ends the running timer is rescaled back to the ShieldType rate. The values apply only to ShieldTypes allowed by `Shield.SelfHealing.Types`, then `Shield.AffectTypes`.

### Respawn

- Runs while the shield is broken, online and enabled, with the same rules for timer, `RestartInCombat`, delay and warhead modifiers as self-healing (`Respawn.*`, `Shield.Respawn.*`).
- When the timer ends the shield returns with the `Respawn` amount (a fraction of `Strength` for -1.0 to 1.0, otherwise points). Only positive amounts respawn. The amount is not capped to `Strength`.
- A respawn plays one random `Respawn.Anim` (or the warhead's `Shield.Respawn.Anim` while a modifier runs) and detonates `Respawn.Weapon` at the owner (or `Shield.Respawn.Weapon`). The tint is refreshed.
- A broken shield with no way to respawn (`Respawn` of 0 and no running warhead modifier with a non-zero amount) is "broken and non-respawning".
- While the shield is broken but can respawn, damage to the owner that counts as combat restarts the respawn timer.

### Enabling, power and EMP

- `SelfHealing.EnabledBy`: self-healing and respawn run only while the owner's house has at least one listed building that is not deactivated, not under EMP and has its power online. The check uses the current owner each update. While not enabled the running timer is paused.
- `Powered=yes`: the shield is offline when the owner is deactivated or under EMP, and for a building also when its power is offline. An offline shield counts as absent for damage, armor and targeting, keeps its strength, pauses its respawn or self-healing timer, drops its tint, and applies `IdleAnim.OfflineAction`. When the owner is back online, the shield, its timer and the animation resume. Units have no power need, so for them only deactivation and EMP count. The Phobos docs add that a building that is not itself `Powered=yes` is never offline for low power; the code asks the building whether its power is online.
- Forced damage from the EMP threshold bypasses the shield.

### Temporal, cloak, burrow, transport, owner change

- Temporal: each frame the owner is under a temporal beam the shield is flagged. While flagged it takes no damage and passes the whole hit to hit points; it pauses its respawn timer (broken) or self-healing timer (otherwise) and applies `IdleAnim.TemporalAction`. The flag clears on the next update after the beam ends and the timer resumes.
- Cloak: the shield still absorbs while the owner is cloaked or cloaking. The idle animation is removed while cloaked if its AnimationType has `DetachOnCloak=yes` (the default) and is created again after uncloaking. Hit, break and respawn animations are not affected.
- Burrowed (tunnel locomotor): the idle animation is removed and hit and break animations are suppressed until the unit surfaces. The respawn animation is not suppressed.
- Transport: entering limbo removes the idle animation. The update stops, but the timers are frame based and keep running, so after the owner leaves the transport a due step runs once without catching up on missed steps.
- Owner change (capture, mind control): Phobos has no shield code for it. The shield keeps its strength and timers. `SelfHealing.EnabledBy` and `Tint.VisibleToHouses` are judged against the current owner. The base-under-attack alert follows the local player's ownership at the time of the hit.
- Chronoshift: Phobos has no shield code for it. The shield state stays with the unit, and the idle animation is made again by the next update if the move removed it.

### Type conversion and deploy

- `DeploysInto`, `UndeploysInto` and Ares `Convert.*` move the shield to the new type. A building that undeploys has its shield state copied to the new vehicle and then cleared from the building.
- If the old shield has `AllowTransfer=no` (the default for the type's own shield; for deploy only `AllowTransfer` is consulted, for `Convert` first `AllowTransfer.Convert`), it is replaced by the new type's `ShieldType` if that has `Strength` above 0, and deleted otherwise. If `AllowTransfer=yes` it stays with its type even when the new type lists no shield.
- When the shield switches to a different type, a non-broken shield keeps its relative strength (`HP * newStrength / oldStrength`, rounded); a broken one stays broken. A shield that keeps its own type (`AllowTransfer=yes`) keeps its absolute strength.
- Timers move across: a running respawn or self-healing timer is rescaled by the ratio of the new rate to the old rate; if the new type has no respawn or self-healing the timer stops; if the old type had none, the timer starts at the new rate unless a combat delay is running. Warhead modifier timers and combat delay timers keep their remaining frames.
- Powered and `EnabledBy` states are recomputed for the new type, and the idle animation is rebuilt if the new type picks a different one.

### Warhead effects (per-target)

- Where they run: with `ApplyPerTargetEffectsOnDetonate=yes` (the default) they run once per target when the warhead detonates, before the area damage, on the targets found by the cell-spread scan, on the bullet's target if within 64 leptons, or on the area-damage target. With `no` they run inside each damage call, before the shield step. They also need the warhead to have at least one of: `Shield.Break`, a respawn or self-healing duration, attach or remove lists, or `Shield.RemoveAll`.
- Targets must pass the warhead's house, health, veterancy and invoker filters. With `EffectsRequireVerses=yes` (the default) they also need a non-zero `Verses` against the armor in effect before the effects run (the shield's armor if it is active and not penetrated), and air or ground permission.
- Order: remove, attach, then break, respawn modifiers, self-healing modifiers.
- Remove: a shield whose type is in `Shield.RemoveTypes` (its list position is remembered) or any shield with `Shield.RemoveAll=yes` is deleted.
- Attach: the first type in `Shield.AttachTypes` is used. With `Shield.ReplaceOnly=yes` the attach type is the one at the position of the removed type (the last when the list is shorter), or the first type when `Shield.RemoveAll` removed a shield not in `Shield.RemoveTypes`; with no removal nothing is attached. A new shield is attached only if its `Strength` is above 0 and the target has no shield, or (with `Shield.ReplaceNonRespawning=yes`) the target's shield is broken, non-respawning and broke at least `Shield.MinimumReplaceDelay` frames ago. A new shield starts at its `InitialStrength`, is marked as attached (so `AllowTransfer` defaults to yes) and has no timers running. With `Shield.ReplaceOnly=yes` and `Shield.InheritStateOnReplace=yes` it takes `Strength` times the old shield's relative strength (a broken old shield gives a broken new one) and its timers start.
- Break: breaks an active shield of an allowed type before damage. A broken, offline or missing shield is unaffected.
- Respawn and self-healing modifiers apply when the duration is above 0 or the matching `RestartTimer` is `yes`, and the shield type is allowed by the matching `.Types` list or `Shield.AffectTypes`.

### Display

- A shielded owner shows a second bar next to its health bar. For non-buildings it is placed from the select bracket at an offset of `PixelSelectionBracketDelta + BracketDelta - 3` pixels (negative is up), which puts it just above the health bar; infantry bars have 8 pips, other units 17. For buildings the bar is a row of pips along the building edge with as many pips as the health bar.
- The number of filled pips is the shield ratio times the length, rounded, at least 1 while the shield is active. Pip frame: green above the yellow threshold, yellow above red, otherwise red. The frames come from `Pips` (units) or `Pips.Building`, then the `[AudioVisual]` counterparts; a single value applies to all three states, a missing yellow or red entry falls back to the first. With nothing set the frame is 16 (units) or 5 (buildings), zero-based.
- For non-buildings a frame is drawn behind the pips only while the owner is selected: frame 3 for infantry and frame 2 for other units in the background SHP, or frame 1 and frame 0 if the file has two frames or fewer.
- A broken shield that can respawn shows an empty bar (empty pips for buildings use `Pips.Building.Empty`). It is hidden when the shield is broken and `Pips.HideIfNoStrength=yes`, or when it is broken and non-respawning.
- The idle animation is chosen from `IdleAnim` and its yellow and red variants by the shield's strength ratio. If the owner is at or below the game's `ConditionYellow` the `IdleAnimDamaged` set is tried first and `IdleAnim` is the fallback. Red falls back to yellow, then to the base. It loops, is attached to the owner, and is made again whenever it is missing and the shield is online, above 0 and not hidden.
- The tint applies while the shield is active, to the houses allowed by `Tint.VisibleToHouses`.
- Visual effects (flash, damage numbers, alert, EVA) are local to each client.

### Multiplayer and determinism

- All shield timing is in frames. The only random draw is the choice among several listed animations (`BreakAnim`, `HitAnim`, `Respawn.Anim` and the warhead versions), which Phobos takes from the synchronized game generator on every client whenever the list is non-empty. A burrowed owner skips the break and hit draws, which every client can compute. The draw order must be the same on every client.
- Break and respawn weapons are detonated through the normal weapon path, so their own random draws also follow.

## What stock YR does

The only hit-point pool is the object's `Strength`. Damage goes through the ArmorBias and veteran armor factors, the Iron Curtain test, the warhead's `Verses` against the object's armor and the distance falloff, then lowers `Strength`. Healing raises it, capped at the maximum. There is no second pool, no armor swap, and no warhead action that removes or attaches protection. The Force Shield is an Iron Curtain variant (`IsForceShielded` on the same timer), not a pool.

## Where it hooks in OpenYR

The engine has no shield code. `grep` of `code/` for the word finds only the Force Shield superweapon (`IsForceShielded`, `SuperClass::Force_Shield`, `ForceShield*` rules). What exists and can be reused:

- Armor types are in place: `ArmorType`, `Armor_From_Name`, `Read_Armor_Types` (`code/armortypes.cpp`, called first in `RulesClass::Addition`, `code/rules.cpp`), and `WarheadTypeClass::Versus`, `Can_Force_Fire`, `Can_Retaliate`, `Can_Passive_Acquire` (`code/warhead.cpp`). A ShieldType `Armor` is read with `CCINIClass::Get_ArmorType` (`code/ccini.cpp`).
- `AttachedEffectsClass` (`code/attacheffect.h`, `.cpp`) is the model for per-object state: it lives in `TechnoClass` (`code/techno.h`), ticks in `TechnoClass::AI`, is dropped in `TechnoClass::Limbo`, and has `Serialize` and `Compute_CRC`. Its warhead effect is applied inside `TechnoClass::Take_Damage` when the warhead has a non-zero `Verses` against the target.
- `CDTimerClass<FrameTimerClass>` (`code/timer.h`) counts frames and serializes. It has stop and start but no separate "never started" state, so the shield needs a flag for that.
- `TechnoClass::StunDuration` is the EMP counter, `TechnoClass::IsDeactivated` the deactivation flag, `BuildingClass::Is_Powered_On` the building power test (it already includes `StunDuration > 0`), `TechnoClass::Cloak` with `CloakType` (`code/cloak.hh`) the cloak state, `TechnoClass::IsBeingWarpedOut` the temporal state, `TechnoClass::IsInLimbo` the limbo state.

### The damage path

1. Area damage: `Explosion_Damage` (`code/combat.cpp`) lists every object in the blast with its distance, then calls `object->Take_Damage(damage, distance, warhead, source)` for each. `BulletClass::Detonate` (`code/bullet.cpp`) calls it, and so do animations, superweapons, rockets and disk lasers. Temporal warheads never reach it; `BulletClass::Detonate` starts the warp instead. `Explosion_Damage` returns early when `strength` is 0 (unless the warhead is webby), so a zero-damage warhead with only shield effects does nothing today.
2. Direct callers pass their own flag. C4 warhead kills, EMP threshold kills and falling vehicles pass `forced = true`; radiation damage (`code/foot.cpp`) is not forced, so a shield on top of the placement below would absorb it.
3. Virtual dispatch: `BuildingClass::Take_Damage`, `InfantryClass::Take_Damage`, `UnitClass::Take_Damage` and `AircraftClass::Take_Damage` do their own work, then call `FootClass::Take_Damage` or `TechnoClass::Take_Damage` (`code/techno.cpp`), which calls `ObjectClass::Take_Damage` (`code/object.cpp`).
4. `TechnoClass::Take_Damage` order today: bunker test; for non-forced damage the ArmorBias, attached-effect and veteran armor division and the type-immune test; radiation, psionic and poison immunity; `AffectsAllies=no`; attached-effect attach; Iron Curtain; delay-kill; Psychedelic; parasite victim notice; then `ObjectClass::Take_Damage`.
5. `ObjectClass::Take_Damage` applies `Modify_Damage` (`code/combat.cpp`: falloff, then `Versus(armor)`, then `MaxDamage`), subtracts from `Strength`, and handles destruction and triggers. `Modify_Damage` treats negative damage as full strength within 8 leptons and 0 beyond; the shield healing path needs the positive formula, so call it with the negated value as Phobos does.

### Where the pool must be consulted

| Place | What changes |
|---|---|
| `TechnoClass::Take_Damage` | New shield step for non-forced damage. Insert it after the bunker test and before the `ArmorBias` division, so that the shield sees the raw damage (the division is what `ApplyArmorMult` reproduces). Use `Modify_Damage` with the shield's armor. When the result for hit points is 0, set `damage = 0` and continue so the retaliation, scatter and shimmer code at the end still runs; do not return early. `ObjectClass::Take_Damage` returns `RESULT_NONE` for 0 damage. Skip the shield when `Is_Iron_Curtained()`, when the type is `IsImmune`, and for same-type same-house type immunity. |
| Alert code | `BuildingClass::Take_Damage` calls `House->Attacked` only when the result is not `RESULT_NONE`. The shield step must trigger it when the shield absorbed damage and the owner is a building, as Phobos' `ResponseAttack` does. |
| `TechnoClass::Can_Fire` | `Can_Force_Fire(techno->Class_Of()->Armor)` must use the effective armor. |
| `TechnoClass::Evaluate_Object` | `Can_Passive_Acquire(object->Class_Of()->Armor)` must use the effective armor. The heal test `object->HealthRatio == Rule->ConditionGreen` must treat a shield below full strength as in need of healing when the weapon is a healing weapon whose warhead does not penetrate the shield. |
| `TechnoClass::Is_Allowed_To_Retaliate` | `Can_Retaliate(source->TClass->Armor)` uses the source's effective armor against the retaliator's warhead. |
| `TechnoClass::Target_Threat` | Both `Versus(...)` terms use the effective armor. |
| `TechnoClass::What_Weapon_Should_I_Use` | The two `Versus(armor) == 0.0` tests use the effective armor against the weapon's own warhead. Phobos also falls back to the secondary weapon when the primary cannot target the shield. |
| `TechnoClass::Base_Is_Attacked`, `UnitClass::Jellyfish_AI` | They read `Versus(...->TClass->Armor)` for help recruitment and damage estimates; use the effective armor if the same treatment as Phobos' shrapnel and evaluation hooks is wanted. |
| `TechnoClass::Draw_Health_Bar` (and `Draw_Health_Bar_Old`) | Draw the shield bar after the health bar. `ObjectTypeClass::PipShapes` and `PipBorderShapes` are the pips and border sheets the health bar already uses; the health bar uses frames 16, 17, 18 for units, so Phobos' default shield frame 16 is the same frame as the green health pip. |
| `TechnoClass::AI` | Call the shield update next to `AttachedEffects.AI(this)`. |
| `TechnoClass::Limbo` | Remove the idle animation (next to `AttachedEffects.Limbo(this)`). |
| `TemporalClass::Update` (`code/temporal.cpp`) or `IsBeingWarpedOut` | Source of the temporal state. |
| `EMPulseClass::Stun` (`code/empulse.cpp`) | EMP sets `StunDuration`; the shield reads `StunDuration > 0` for `Powered`. |
| Deploy and undeploy | `UnitClass` deploy code (`new BuildingClass(Class->DeploysInto, House)`, `code/unit.cpp`) and the undeploy path in `code/building.cpp` (the place that calls `Transfer_Slaves`) are the two places to move a shield, next to `TechnoClass::Transfer_Slaves` (`code/techno.cpp`). |
| `Explosion_Damage` (`code/combat.cpp`) | Warhead per-target effects (remove, attach, break, modifiers) belong in the `Explosion_Damage` hit loop right before `Take_Damage`, which covers every area-damage caller and sees the distance. |

### Build order

1. Read the data: `[ShieldTypes]` and ShieldType sections (new `code/shieldtype.h/.cpp` modeled on `armortypes.cpp`, read in `RulesClass::Addition` after `Read_Armor_Types` and before the type reads), `ShieldType=` in `TechnoTypeClass` (`code/techtype.cpp`), the warhead keys in `WarheadTypeClass` (`code/warhead.cpp`), the globals in `RulesClass`. Add the type data to `Serialize` and `Compute_CRC`, and read `ShieldType=` so that an unlisted name is reported in the debug log.
2. Add `ShieldClass` (HP, type, flags, timers, warhead modifier values) as a member of `TechnoClass`, created from the type and serialized with it. No behaviour yet.
3. Core damage step in `TechnoClass::Take_Damage`: `Armor`, `Strength`, `AbsorbPercent`, `PassPercent`, `ReceivedDamage.*`, `AbsorbOverDamage`, break, healing absorption, `Shield.Penetrate`, Psychedelic penetration, Iron Curtain, `Immune`, type immunity. Add the shield bar to `Draw_Health_Bar`. This is a usable first milestone.
4. Effective armor in targeting (the table above), `InheritArmorFromTechno` and its lists, and `ApplyArmorMult`.
5. The per-frame update: self-healing, respawn, the `RestartInCombat` timers, `EnabledBy`, `Powered` with EMP, temporal and cloak handling.
6. Warhead per-target effects: `Shield.Break`, attach, remove, `Shield.Respawn.*`, `Shield.SelfHealing.*`, and the `.Types` lists. Relax the `Explosion_Damage` zero-strength early return for warheads that carry shield effects.
7. Animations and effects: idle, break, hit, respawn animations, break and respawn weapons, hit flash, tint.
8. Type conversion on deploy and undeploy (`AllowTransfer`), and the map trigger event 600.
9. Optional extras outside this page: the digital display `InfoType=Shield`, damage numbers, `ImmuneToCrit` (needs the crit feature), `PenetratesIronCurtain` and `PenetratesForceShield` (not yet in the engine).

## Saved state

Per object, in `TechnoClass::Serialize` and `Compute_CRC` (`code/techno.cpp`):

- Shield present flag and the current ShieldType (index or pointer, as `TechnoClass` saves other type pointers). The current type can differ from the TechnoType's `ShieldType` after attach, remove or conversion.
- Strength (integer) and the TechnoType the shield was last built for (used by `BracketDelta`).
- Flags: cloak, online, temporal, attached, animations hidden, self-healing enabled.
- Six timers: self-healing, self-healing combat delay, self-healing warhead modifier, respawn, respawn combat delay, respawn warhead modifier. Each needs the remaining time and the started, paused or never-started state.
- Warhead modifier values: self-healing amount, rate, restart-in-combat flag and delay; respawn amount, rate, restart-in-combat flag and delay, animation list and weapon.
- The frame the shield last broke and the owner's last health ratio (used for the damaged idle animation).
- The idle animation pointer. Rebuild it after load instead of saving it.

Per type and global: all ShieldType keys, the new warhead keys and the six `[AudioVisual]` and one `[CombatDamage]` values, in the type serializers and in the type CRC. The types sit in the same place as the armor-type data.

Replays and networking: no new events are needed. All shield state comes from existing orders and frame counts, so it belongs in `Compute_CRC`: a differing shield value changes later damage, and a desync should show up on the frame it happens. No Phobos hook adds shield state to the game CRC, so this is a deliberate addition. Raise `SaveVersionInfo::REVISION` in `code/savever.h` (see `docs/SAVE-FORMAT.md`), because the `Serialize` layout changes.

## Open questions

1. Phobos' `IsImmobilized` flag on the owner makes the shield update return without ticking. Its exact meaning is in the Phobos headers (YRpp), which are not available here. The engine's `TechnoClass::Is_Immobilized` is true under EMP stun, deactivation and a locomotor hold. Using it would pause the timers of a non-`Powered` shield during EMP. Decide whether to use `IsInLimbo` only.
2. Where to consult the shield (owner decision). Phobos hooks the first instruction of the damage routine, so its shield also absorbs damage that the stock code would then ignore (radiation, psionic and poison immunity, `AffectsAllies=no`). The placement recommended here, after the bunker test and before the `ArmorBias` division, matches that. Consulting the shield after those immunity tests would spare the shield from hits the owner ignores, but it would need the `ArmorBias` division moved below the shield step, and it differs from Phobos.
3. Residual damage on the breaking hit is converted back by dividing by the shield armor's `Verses`, but the leftover already includes distance falloff, and the stock code applies the falloff again. A splash warhead with `PercentAtMax` below 1 therefore seems to lose extra damage on the breaking hit. Confirm with a test and decide whether to copy it.
4. A temporal beam on the owner makes the shield pass the whole hit to hit points. The docs word this as "the shield does not take damage". Confirm in game, since it lets units under a beam be killed through their shield.
5. `ReceivedDamage.Minimum` above 0 also clamps healing: a healing hit of -10 becomes damage. Phobos does not guard against this.
6. `Respawn` above `Strength` (for example `Respawn=500` on a 100-point shield) sets the strength above the maximum, with no cap, until the next self-healing step caps it. The bar then shows full.
7. `Shield.Respawn.RestartTimer=yes` or `Shield.SelfHealing.RestartTimer=yes` with a duration of 0 is ignored unless the warhead also sets another shield effect, because the pre-check list in `WarheadTypeExt` leaves those flags out.
8. A ShieldType named in `ShieldType=` but missing from `[ShieldTypes]` is created with defaults. Decide whether OpenYR should still read its section (friendlier) or copy this behaviour.
9. Unknown names in the list keys (`Shield.AttachTypes`, `InheritArmor.Allowed` and the others) are reported by the Phobos reader. Decide whether the engine reader keeps the other entries of the list or rejects the whole key.
10. The default shield pip frame (16 for units) is the same frame this engine draws for green health. Mods normally set `Pips.Shield`. Confirm the original game's `pips.shp` frame 16 to see whether the default looks like health.
11. `Tint.Cumulative` and `.pcx` for `Pips.Background` are parsed or documented but have no visible effect on shields.
12. Where the docs and the code disagree (listed after the key tables), the recommended OpenYR behaviour is to follow the code, so existing Phobos mods behave the same, except for `InheritArmor.Disallowed`, where the docs describe the more useful rule. Owner decision.
13. Chronoshift and owner change: Phobos has no shield rules for either. If OpenYR wants a shield reset on capture or mind control, that is a deliberate difference.
14. Force Shield and Iron Curtain: a shield never absorbs damage the owner is immune to, because the shield step is skipped for an invulnerable owner. The Force Shield and Iron Curtain tint and organic-unit settings (`ForceShield.*`) are separate Phobos features.

## Test plan

Common setup. In a test mod's rules, add:

```ini
[ShieldTypes]
0=TESTSHIELD

[TESTSHIELD]
Strength=100

[TESTWH]
Verses=100%,100%,100%,100%,100%,100%,100%,100%,100%,100%,100%
CellSpread=0
Damage=50

[TESTGUN]
Damage=50
ROF=60
Range=6
Projectile=Invisible
Warhead=TESTWH
```

Give `Primary=TESTGUN` to the Allied GI (`E1`) and the Soviet Conscript (`E2`), set `Strength=100` and `Armor=none` on both, and add `ShieldType=TESTSHIELD` to `[E1]`. Put the two on opposing houses, force-fire an `E2` at an `E1` and count the shots until the target dies. The expected counts assume every shot hits and the target starts with full health and a full shield. If the owner has a Phobos build, the same INI run there gives the reference.

1. Shield absorbs first. Run the setup as written. Expected: the GI dies on shot 4. Shot 1 leaves the shield at 50, shot 2 breaks it without touching hit points, shot 3 takes the GI to 50 and shot 4 kills it. The infantry shield bar has 8 pips and drops to half after shot 1. Remove `ShieldType=` from `[E1]` and repeat: the GI dies on shot 2.
2. Break residue and `AbsorbOverDamage`. Set `Strength=30` in `[TESTSHIELD]` and `[E1] Strength=120`. With `AbsorbOverDamage=no`: shot 1 breaks the shield and takes 20 from hit points (100 left), shot 2 leaves 50, shot 3 kills. With `AbsorbOverDamage=yes`: shot 1 leaves 120, shot 2 leaves 70, shot 3 leaves 20, shot 4 kills.
3. Shield armor and the residue conversion. Set `Armor=special_1` and `Strength=110` in `[TESTSHIELD]`, change `TESTWH` to `Verses=100%,100%,100%,100%,100%,100%,100%,100%,100%,50%,100%` (50% is the tenth entry, `special_1`), and set `[E1] Strength=80`. Each shot does 25 to the shield. Shot 5 meets 10 points of shield, breaks it, and passes `(25 - 10) / 0.5 = 30` points on, which the GI's `none` armor takes at 100% (50 left); shot 6 kills. Without the division by the shield armor's `Verses` the leftover would be 15 and the GI would die on shot 7.
4. `PassPercent`. Restore the setup, add `PassPercent=0.5` to `[TESTSHIELD]`. Shot 1 takes the shield to 50 and the GI to 75; shot 2 breaks the shield (0 residue) and takes the GI to 50; shot 3 kills. Without `PassPercent` the GI dies on shot 4.
5. `Shield.Penetrate`. Make `[TESTWH2]` a copy of `TESTWH` with `Shield.Penetrate=yes` and a weapon `TESTGUN2` that uses it. Give `E2` `TESTGUN2`, set the shield `Strength=1000`. Expected: the GI dies on shot 2 and its shield bar stays full. Edge: give the shield `Armor=special_1`, make a copy of `TESTWH` with 0% against `special_1` and no penetration, and give it to a weapon: the Conscript cannot attack the shielded GI (no attack cursor, no automatic targeting). Add `Shield.Penetrate=yes` to that warhead and the Conscript attacks.
6. Respawn and `RestartInCombat`. Set `Respawn=1.0` and `Respawn.Rate=0.1` (90 frames). Break the shield with two shots and stop. Expected: the bar refills to full about 90 frames after the break (6 seconds at 15 frames per second) with no further shot. Repeat with `E1 Strength=1000` and fire a shot every 60 frames for 5 shots after the break: the bar stays empty during the volley and refills 90 frames after the last shot. With `Respawn.RestartInCombat=no` it refills on the 90-frame schedule while being shot.
7. Self-healing and the zero-rate edge. Set `SelfHealing=0.5`, `SelfHealing.Rate=0.1`, `E1 Strength=1000`. Shoot once every 100 frames for 10 shots: each shot takes the shield to 50 and 90 frames later it is back at 100, so the GI never loses health. Edge: use `SelfHealing=10` with no rate: the shield gains 10 points every frame, refills between shots, and never breaks.
8. Warhead break and attach. Give `E2` a weapon whose warhead `[BREAKWH]` has `Shield.Break=yes`, `Damage=1`, all `Verses` 100%. After one hit the GI's shield bar is gone; with no `Respawn` it stays gone, and the next `TESTGUN` shots hit hit points directly. Make `[ATTACHWH]` with `Shield.AttachTypes=TESTSHIELD`, `Damage=1`, all `Verses` 100%, and fire it with `E1` at an `E2`: the Conscript gets a full bar and now dies on shot 4 instead of shot 2. Fired at a shielded GI it changes nothing.
9. Powered shield and EMP. Set `Powered=yes` and `Strength=1000` in `[TESTSHIELD]` and give it to a Grizzly tank. Hit the tank with a test EMP weapon (a warhead with `EMEffect=yes`, `Damage=300`, `CellSpread=2`). Expected: while the tank is disabled, `TESTGUN` shots lower its health bar and the shield bar does not move. After the stun ends the shield works again with the strength it had.
10. Transport edge. Set `Strength=200`, `SelfHealing=20`, `SelfHealing.Rate=0.1` and `E1 Strength=1000`. Shoot the GI twice (shield 100) and load it into an APC within 60 frames. Unload it after 900 frames. Expected: within a second of unloading the shield shows 120 (one step), not 200; later steps follow every 90 frames.
11. Deploy keeps the ratio. Give an MCV `ShieldType=A` (`Strength=100`) and the Construction Yard `ShieldType=B` (`Strength=400`). Damage the MCV's shield to 50 and deploy: the yard's shield is at 200. Sell-undeploy it: the MCV's shield is at 50. Then set `AllowTransfer=yes` on `A` and remove `ShieldType` from the yard: the yard keeps shield `A` at 50 of 100.
12. Owner change. Put `SelfHealing.EnabledBy=GAPOWR` on a shield with a rate of 0.1, damage a shielded unit of player 1, and mind-control it with a Mastermind of player 2 who owns no power plant. Expected: the shield keeps its current strength and does not heal. Build a power plant for player 2: it heals. A power plant owned only by player 1 does not count.
13. Temporal edge. Freeze a shielded GI with a Chrono Legionnaire, then shoot it with `TESTGUN`. Expected (per the Phobos source, to be confirmed): the shield bar does not move and hit points fall.
14. Unlisted type edge. Put `ShieldType=ORPHAN` on `[E1]` and define `[ORPHAN]` with `Strength=100` in rules.ini but leave it out of `[ShieldTypes]`. Expected from the Phobos source: no shield appears. Add `0=ORPHAN` to the list and it appears.
