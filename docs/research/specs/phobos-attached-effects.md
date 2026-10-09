# Attached effects (Phobos)

Source: Phobos `docs/New-or-Enhanced-Logics.md` section "Attached Effects"; `src/New/Type/AttachEffectTypeClass.*`, `src/New/Entity/AttachEffectClass.*`, `src/Ext/Techno/Body.Update.cpp`, `Hooks.cpp`, `Hooks.Firing.cpp`, `Hooks.ReceiveDamage.cpp`, `Hooks.Cloak.cpp`, `WeaponHelpers.cpp`, `src/Ext/WarheadType/Detonate.cpp`, `src/Ext/WeaponType/Body.cpp`, `src/Ext/House/Hooks.cpp`, `src/Ext/Unit/Hooks.DeploysInto.cpp`.

A Phobos attached effect is a named type declared in `[AttachEffectTypes]`. Technos, countries, weapons and warheads attach instances of these types to objects. Each instance runs a timer, changes stats, plays an animation and can fire a weapon when it ends. The Ares-style effect already in `code/attacheffect.cpp` keeps working unchanged; the Phobos system is a second, richer layer with the same name.

## Keys

Key spellings are the ones the Phobos code reads. List values are comma separated. "House list" means the Phobos affected-house enumeration: `owner` (or `self`), `allies` (or `ally`), `enemies` (or `enemy`), `neutral`, `team` (owner and allies), `others` (allies and enemies), `all`, `none`; several may be combined with commas. "Positional list" means integers matched to the entries of `AttachEffect.AttachTypes` (or `RemoveTypes`) by position, with the last value reused for later entries.

### rules.ini `[General]`

| Key | Type | Default | Notes |
|---|---|---|---|
| `DiscardOn.Sequences.Immediate` | boolean | `true` | Default for effect types that do not set it. |
| `DiscardOn.MoveBasedOnDestination` | boolean | `false` | Default for effect types. |
| `DiscardOn.ConsiderHarvestingAsStationary` | boolean | `true` | Default for effect types. |
| `OpenTopped.UseTransportRangeModifiers` | boolean | `false` | Default for TechnoTypes. |
| `OpenTopped.CheckTransportDisableWeapons` | boolean | `false` | Default for TechnoTypes. |
| `AttachEffect.ReplaceLongerDuration` | boolean | `false` | Default for weapons and warheads. |
| `AttachEffects.AttachOnOwnerChange` | boolean | `false` | The code reads the plural `AttachEffects`; the docs print the singular. See open question 3. |

### rules.ini `[AttachEffectTypes]`

| Key | Type | Default | Notes |
|---|---|---|---|
| `0=`, `1=`, ... | effect type name | none | Each value names a section of the same name. A name with no section keeps all defaults but leaves the tint object unset in Phobos (open question 14). |

### Effect type section (one per name in `[AttachEffectTypes]`, rules.ini)

Duration and stacking:

| Key | Type | Default | Notes |
|---|---|---|---|
| `Duration` | integer frames | `0` | Negative means no end. `0` ends at the next update of the object. |
| `Duration.ApplyFirepowerMult` | boolean | `false` | Duration is multiplied by the invoker's firepower multiplier. |
| `Duration.ApplyArmorMultOnTarget` | boolean | `false` | Duration is divided by the target's armor multiplier. |
| `Cumulative` | boolean | `false` | Allows several instances of this type on one object. |
| `Cumulative.MaxCount` | integer | `-1` | Negative means no limit. |
| `Powered` | boolean | `false` | Effect is inactive while the object is deactivated, under EMP, or (buildings) on low power. |
| `Groups` | list of strings | empty | Free-form, case-sensitive group names used by remove and weapon-filter keys. |

Filters and transfer:

| Key | Type | Default | Notes |
|---|---|---|---|
| `PenetratesIronCurtain` | boolean | `false` | Without it the effect is not attached to an invulnerable object. |
| `PenetratesForceShield` | boolean | value of `PenetratesIronCurtain` | Used instead when the object is force-shielded. |
| `AffectTypes` | list of TechnoTypes | empty | Empty means no whitelist. |
| `IgnoreTypes` | list of TechnoTypes | empty | Blacklist. |
| `AffectsTarget` | list: `none`, `infantry`, `units`, `aircraft`, `buildings`, `all` | `all` | The old spelling `AffectTargets` is read first and `AffectsTarget` overrides it. The parser also accepts `land`, `water`, `empty`, which have no effect here. |
| `AllowTransfer` | boolean | unset: `false` for effects the TechnoType attached to itself, `true` otherwise | Whether the effect survives a type change or deploy. |
| `AllowTransfer.Convert` | boolean | value of `AllowTransfer` | Same, for type conversion only. |

Discard conditions:

| Key | Type | Default | Notes |
|---|---|---|---|
| `DiscardOn` | list: `none`, `entry`, `move`, `stationary`, `drain`, `inrange`, `outofrange`, `firing`, `receiveddamage`, `selling`, `undeploying`, `harvesting`, `invokerdie`, `ammo`, `health`, `mission`, `landtype`, `sequence`, `ownerchange` | `none` | An unknown word rejects the whole value. |
| `DiscardOn.Ammo.MinimumAmount` | integer | `-1` | Inclusive; `-1` ignores the bound. |
| `DiscardOn.Ammo.MaximumAmount` | integer | `-1` | Inclusive; `-1` ignores the bound. |
| `DiscardOn.Health.AbovePercent` | float (fraction or `%`) | `-1` | Exclusive lower bound. |
| `DiscardOn.Health.BelowPercent` | float | `-1` | Inclusive upper bound. See open question 6. |
| `DiscardOn.Firing.Count` | integer | `1` | Shots before discard. |
| `DiscardOn.ReceivedDamage.Count` | integer | `1` | Damage hits before discard. |
| `DiscardOn.ReceivedDamage.AffectsHouse` | house list | `all` | |
| `DiscardOn.Missions` | list of missions | empty | |
| `DiscardOn.AIMissions` | list of missions | value of `DiscardOn.Missions` | Used for objects whose owner is not human-controlled. |
| `DiscardOn.LandTypes` | list of land types | empty | |
| `DiscardOn.Sequences` | list of infantry sequences | empty | |
| `DiscardOn.Sequences.Immediate` | boolean | `[General]` value | |
| `DiscardOn.RangeOverride` | float cells | weapon range | For `inrange` and `outofrange`. |
| `DiscardOn.MoveBasedOnDestination` | boolean | `[General]` value | |
| `DiscardOn.ConsiderHarvestingAsStationary` | boolean | `[General]` value | |
| `DiscardOn.OwnerChange.HumanToComputer` | boolean | `true` | |
| `DiscardOn.OwnerChange.ComputerToHuman` | boolean | `true` | |
| `DiscardOn.OwnerChange.IgnoreRevertOnExit` | boolean | `false` | |

Animation and tint:

| Key | Type | Default | Notes |
|---|---|---|---|
| `Animation` | AnimType | none | Loops while the effect is active. |
| `Animation.ResetOnReapply` | boolean | `false` | |
| `Animation.OfflineAction` | `None`, `Hides`, `Temporal`, `Paused`, `PausedTemporal` | `Hides` | Only with `Powered=yes`. |
| `Animation.TemporalAction` | same values | `None` | |
| `Animation.UseInvokerAsOwner` | boolean | `false` | |
| `Animation.HideIfAttachedWith` | list of effect types | empty | |
| `Animation.DrawOffset<n>` | `x,y` pixels | none | `n` counts from 0; reading stops at the first missing or `0,0` entry. |
| `Animation.DrawOffset<n>.RequiredTypes` | list of effect types | empty | |
| `CumulativeAnimations` | list of AnimTypes | empty | Replaces `Animation` for `Cumulative=yes` types. |
| `CumulativeAnimations.RestartOnChange` | boolean | `true` | |
| `CumulativeAnimations.CountIncrement` | integer | `1` | Values below 1 become 1 with a log warning. |
| `Tint.Color` | `r,g,b` | `0,0,0` | |
| `Tint.Intensity` | float | `0.0` | Additive lighting, `1.0` is normal lighting. |
| `Tint.VisibleToHouses` | house list | `all` | |
| `Tint.Cumulative` | boolean | `true` | |
| `LaserTrail.Type` | LaserTrailType | none | Present in the code and the doc example, not described in the docs prose. |

Expiry weapon:

| Key | Type | Default | Notes |
|---|---|---|---|
| `ExpireWeapon` | WeaponType | none | |
| `ExpireWeapon.TriggerOn` | list: `none`, `expire`, `remove`, `death`, `discard`, `all` | `expire` | |
| `ExpireWeapon.CumulativeOnlyOnce` | boolean | `false` | |
| `ExpireWeapon.UseInvokerAsOwner` | boolean | `false` | |

Stat modifiers:

| Key | Type | Default | Notes |
|---|---|---|---|
| `FirepowerMultiplier` | float | `1.0` | |
| `ArmorMultiplier` | float | `1.0` | Incoming damage is divided by it. |
| `ArmorMultiplier.AllowWarheads` | list of WarheadTypes | empty | Empty means all. |
| `ArmorMultiplier.DisallowWarheads` | list of WarheadTypes | empty | |
| `ArmorMultiplier.Chance` | float 0.0 to 1.0 | `1.0` | |
| `ArmorMultiplier.Delay` | integer frames | `0` | |
| `ArmorMultiplier.AffectsHouse` | house list | `all` | Houses of the attacker. |
| `ArmorMultiplier.HitAnim` | list of AnimTypes | empty | One is picked at random per applied hit. |
| `SpeedMultiplier` | float | `1.0` | |
| `ROFMultiplier` | float | `1.0` | Multiplies the reload delay, so values below 1 fire faster. |
| `ROFMultiplier.ApplyOnCurrentTimer` | boolean | `true` | |
| `Cloakable` | boolean | `false` | |
| `ForceDecloak` | boolean | `false` | |
| `WeaponRange.Multiplier` | float | `1.0` | |
| `WeaponRange.ExtraRange` | float cells | `0.0` | May be negative. |
| `WeaponRange.AllowWeapons` | list of WeaponTypes | empty | Empty means all. |
| `WeaponRange.DisallowWeapons` | list of WeaponTypes | empty | |
| `Crit.Multiplier` | float | `1.0` | Needs the Phobos critical-hit system. |
| `Crit.ExtraChance` | float | `0.0` | |
| `Crit.AllowWarheads` | list of WarheadTypes | empty | Empty means all. |
| `Crit.DisallowWarheads` | list of WarheadTypes | empty | |

Weapons and damage events:

| Key | Type | Default | Notes |
|---|---|---|---|
| `RevengeWeapon` | WeaponType | none | Needs the Phobos revenge-weapon system. |
| `RevengeWeapon.AffectsHouse` | house list | `all` | Old spelling `RevengeWeapon.AffectsHouses` is read first. |
| `RevengeWeapon.UseInvokerAsOwner` | boolean | `false` | |
| `ReflectDamage` | boolean | `false` | |
| `ReflectDamage.Warhead` | WarheadType | `[CombatDamage] C4Warhead` | |
| `ReflectDamage.Warhead.Detonate` | boolean | `false` | The docs call the type WarheadType; the code reads a boolean. |
| `ReflectDamage.Multiplier` | float | `1.0` | |
| `ReflectDamage.AffectsHouse` | house list | `all` | Old spelling `.AffectsHouses` is read first. |
| `ReflectDamage.Chance` | float 0.0 to 1.0 | `1.0` | |
| `ReflectDamage.Delay` | integer frames | `0` | |
| `ReflectDamage.Override` | integer | unset | Fixed reflected damage. |
| `ReflectDamage.UseInvokerAsOwner` | boolean | `false` | |
| `DisableWeapons` | boolean | `false` | |
| `Unkillable` | boolean | `false` | |

### TechnoType section (rules.ini)

| Key | Type | Default | Notes |
|---|---|---|---|
| `AttachEffect.AttachTypes` | list of effect types | empty | Attached to every object of the type when it is created. |
| `AttachEffect.DurationOverrides` | positional list, frames | none | `0` means no override. |
| `AttachEffect.Delays` | positional list, frames | `0` | Negative means the effect is not recreated after it ends. |
| `AttachEffect.InitialDelays` | positional list, frames | `0` | |
| `AttachEffect.RecreationDelays` | positional list, frames | `-1` | Negative means never. |
| `OpenTopped.UseTransportRangeModifiers` | boolean | `[General]` value | |
| `OpenTopped.CheckTransportDisableWeapons` | boolean | `[General]` value | |

The code also reads `RemoveTypes`, `RemoveGroups`, the cumulative keys and the count lists from this section; they do nothing for a TechnoType.

### Country section (rules.ini, house types)

| Key | Type | Default | Notes |
|---|---|---|---|
| `AttachEffect.AttachTypes` | list of effect types | empty | Attached to every object created for this country, subject to each effect's filters. |
| `AttachEffect.DurationOverrides`, `.Delays`, `.InitialDelays`, `.RecreationDelays` | positional lists | as for TechnoTypes | |
| `AttachEffect.AttachOnOwnerChange` | boolean | `[General] AttachEffects.AttachOnOwnerChange` | Singular here, plural in `[General]`. |

### WeaponType and WarheadType sections (rules.ini)

| Key | Read in | Type | Default | Notes |
|---|---|---|---|---|
| `AttachEffect.AttachTypes` | weapon, warhead | list of effect types | empty | |
| `AttachEffect.DurationOverrides` | weapon, warhead | positional list | none | |
| `AttachEffect.CumulativeSourceMaxCount` | weapon, warhead | integer | `-1` | Docs list it for warheads only. |
| `AttachEffect.CumulativeRefreshAll` | weapon, warhead | boolean | `false` | |
| `AttachEffect.CumulativeRefreshAll.OnAttach` | weapon, warhead | boolean | `false` | |
| `AttachEffect.CumulativeRefreshSameSourceOnly` | weapon, warhead | boolean | `true` | |
| `AttachEffect.ReplaceLongerDuration` | weapon, warhead | boolean | `[General]` value | Docs list it for warheads only. |
| `AttachEffect.RemoveTypes` | weapon, warhead | list of effect types | empty | |
| `AttachEffect.RemoveGroups` | weapon, warhead | list of strings | empty | |
| `AttachEffect.CumulativeRemoveMinCounts` | weapon, warhead | positional list | none | |
| `AttachEffect.CumulativeRemoveMaxCounts` | weapon, warhead | positional list | none | `0` or less removes without a limit. |
| `AttachEffect.RequiredTypes` | weapon | list of effect types | empty | |
| `AttachEffect.DisallowedTypes` | weapon | list of effect types | empty | |
| `AttachEffect.RequiredGroups` | weapon | list of strings | empty | |
| `AttachEffect.DisallowedGroups` | weapon | list of strings | empty | |
| `AttachEffect.RequiredMinCounts`, `.RequiredMaxCounts` | weapon | positional lists | none | Only for `Cumulative=yes` types. |
| `AttachEffect.DisallowedMinCounts`, `.DisallowedMaxCounts` | weapon | positional lists | none | Only for `Cumulative=yes` types. |
| `AttachEffect.Required.Any` | weapon | boolean | `false` | |
| `AttachEffect.Disallowed.Any` | weapon | boolean | `true` | |
| `AttachEffect.Required.Houses` | weapon | house list | `all` | |
| `AttachEffect.Disallowed.Houses` | weapon | house list | `all` | |
| `AttachEffect.IgnoreFromSameSource` | weapon | boolean | `false` | |
| `AttachEffect.SameSourceOnly` | weapon | boolean | `false` | |
| `AttachEffect.CheckOnFirer` | weapon | boolean | `false` | |
| `SuppressReflectDamage` | warhead | boolean | `false` | |
| `SuppressReflectDamage.Types` | warhead | list of effect types | empty | |
| `SuppressReflectDamage.Groups` | warhead | list of strings | empty | |

The warhead section also reads `AttachEffect.Delays`, `.InitialDelays` and `.RecreationDelays`; they are ignored there.

### Keys of other sections this feature reads

| Key | Section | Notes |
|---|---|---|
| `DetachOnCloak` | AnimType (default `true`) | Owned by the animation group. An effect animation is removed from a cloaked object only when its AnimType has `DetachOnCloak=yes`. |
| `[LaserTrailTypes]` | art.ini | Owned by the laser trail group; needed by `LaserTrail.Type`. |
| Map trigger event 606, "Attached Is Under Attached Effect" | trigger events | Parameter 2 is an effect type name. True while the object holds an active instance of the type; also raised when an instance is attached, refreshed or finishes a delay. |

## Behaviour

### What is new relative to the Ares-style effect

The engine's Ares-style effect (`AttachEffect.*` on a warhead or TechnoType, see `specs/attach-effect.md`) and the Phobos effect are separate systems. Phobos does not read the Ares keys. A mod can use both on one object. Both change the same stats, so the port should multiply the multipliers of the two systems.

| Subject | Ares-style, in the engine now | Phobos, new |
|---|---|---|
| Definition | One anonymous effect per warhead or TechnoType, keys inline. | Named types in `[AttachEffectTypes]`; each source lists several. |
| Sources | Warhead hit, own TechnoType. | Adds weapons (at firing), countries, owner change. |
| Warhead trigger | Inside `Take_Damage`, only when Verses is not 0%. | At warhead detonation, with the warhead's house, air/ground, health and veterancy filters; Verses unless `EffectsRequireVerses=no`. |
| Refresh identity | By warhead. | By effect type; cumulative types add source and count limits. |
| Cumulative | Unlimited instances. | `Cumulative.MaxCount`, per-source cap, refresh-all and same-source rules. |
| Ending | Duration, or `DiscardOnEntry`. | Eighteen discard conditions, positional delays, recreation delays, expiry weapon. |
| Animation | Hidden whenever the object is cloaked; `TemporalHidesAnim`. | Hidden on cloak only if the AnimType says so; offline and temporal actions; draw offsets; per-count animations. |
| Stats | Speed, armor, firepower, ROF multipliers, cloak. | Same four, plus conditional armor multiplier, range, critical-hit chance, tint, `DisableWeapons`, `Unkillable`, laser trail. |
| Events | None. | Expiry, revenge and reflect weapons; weapon firing filters; removal by type or group; map trigger event. |
| Type change | Effects are lost when a deploy creates a new object. | `AllowTransfer` carries effects across deploy, undeploy and conversion. |

### Application

- Four things attach instances. A TechnoType and a country attach their lists when an object of that type or country is created, with the object as invoker and source. A weapon attaches its list each time it fires at a techno, before the projectile exists, with the firer as invoker and the weapon's warhead as source. A warhead attaches its list to each object its detonation affects, with the firer as invoker and the warhead itself as source. The invoker may be absent (for example a warhead from a superweapon); the invoker house is kept.
- A weapon aimed at a cell or non-techno target attaches nothing. Weapon-attached effects ignore the warhead's house, air/ground and Verses filters. Only the effect type's own filters apply, and the effect is attached even if the projectile is later intercepted or misses.
- Warhead path: the object must be alive, not in limbo, not sinking and not being warped out, and must pass the warhead's usual target tests. A warhead with `CellSpread=0` affects only its bullet's own target and only if the detonation is within a quarter cell of it. A warhead with a cell spread affects every object in the spread. The attach runs before the warhead's damage is dealt, so the hit that attaches an armor effect is already reduced by it. Within one warhead or weapon, the order is: attach the list, then `RemoveTypes`, then `RemoveGroups`.
- Each listed type must pass all of these, in any order: the object is not invulnerable (or the type has the matching `Penetrates*` key), the object's kind matches `AffectsTarget` (a building flagged as strange counts as a unit), `AffectTypes` is empty or lists the object's type, and `IgnoreTypes` does not list it. A type that fails is skipped and the rest of the list continues. A name that is not a declared effect type is dropped when the list is read.
- Duration comes from `DurationOverrides` at the type's position (`0` means no override), otherwise from `Duration`. With `Duration.ApplyFirepowerMult` and a positive duration, the duration is multiplied by the invoker's current firepower multiplier (house, techno, veterancy, effects, and occupant, bunker and open-topped bonuses). With `Duration.ApplyArmorMultOnTarget` and a positive duration, it is divided by the target's current armor multiplier including this effect's own `ArmorMultiplier` and ignoring the warhead filters of that multiplier. Results are truncated and never below 0.
- A `Duration` of 0 gives an instance that exists until the object's next update. Its multipliers apply to anything resolved in between, then it is removed (and an `expire` weapon fires). Use it for a one-shot weapon trigger.

### Stacking and refresh

- A non-cumulative type has at most one instance per object, whatever the source. A new application refreshes it when the new duration (override or type duration) is at least the remaining time, or when `ReplaceLongerDuration` is on. A refresh sets the remaining time to the override of this application, else the override stored in the instance, else `Duration`. If the instance is not refreshed, only `Animation.ResetOnReapply` has an effect. A refresh creates no new instance and is not counted as an attach.
- For `Cumulative=yes`, "same source" means the same invoker and the same source object (warhead, weapon's warhead, or TechnoType). Counts include every instance of the type on the object, even ones that are waiting for a delay.
- Below both caps (`Cumulative.MaxCount` for the type, `CumulativeSourceMaxCount` per source) the application adds a new instance. With `CumulativeRefreshAll` and `CumulativeRefreshAll.OnAttach` it also refreshes every eligible instance.
- At either cap no instance is added. The application refreshes the eligible instance with the least time left (or every eligible instance with `CumulativeRefreshAll`, which ignores `ReplaceLongerDuration`). Eligible means same source when `CumulativeRefreshSameSourceOnly` is on (the default), otherwise any instance of the type. If nothing is eligible, nothing happens. `Cumulative.MaxCount=0` therefore blocks the type entirely.
- An instance with no end (negative remaining time) counts as the shortest when picking the one to refresh.
- Listing a cumulative type several times in one `AttachTypes` adds one instance per entry, up to the caps, each with its own positional override.
- Different effect types, and instances from different sources of a non-cumulative type, never merge.

### Duration and expiry

- Each instance counts down once per update of its object, at the start of the techno update. The count does not run while the object is in limbo (inside a transport or structure, or off the map) or flagged immobilised. A negative duration never ends.
- An effect that was attached by a TechnoType or a country follows its delay lists. When `Duration` reaches 0: a negative delay removes the effect; a delay of 0 restarts the duration at once, so with the default `Delays` the effect never ends, never counts as expired, and never fires an expiry weapon; a positive delay makes the effect inactive for that many frames, then restarts it. The delay does not count down while a discard condition holds. Warhead and weapon effects always end at 0 and ignore all three delay lists.
- `InitialDelays` keeps a new instance inactive for that many frames. The Phobos code counts this down every update; the docs say it pauses while a discard condition holds (open question 5).
- `RecreationDelays` applies when an instance is removed by `RemoveTypes`/`RemoveGroups` or by a discard condition. With a value of 0 or more the instance stays, inactive, and comes back after that many frames (the countdown pauses while a discard condition holds). With the default `-1` the instance is deleted for good.
- Inactive means: still in the list, no stat modifiers, no animation, not counted by weapon filters or the trigger event.
- Discard conditions are tested once per frame per instance, only while the instance is active (power is ignored). Results are cached for the frame.

| Condition | True when |
|---|---|
| `entry` | The object leaves the map by any limbo, such as entering a transport or structure. Skipped for a building that is undeploying. Tested when the object is limboed, not each frame. |
| `move` / `stationary` | Foot objects only. Movement is "really moving now" by default, or "has a destination" with `MoveBasedOnDestination` (hovering jumpjets count as moving, units turning in place as stationary). With `ConsiderHarvestingAsStationary=yes` a harvesting object counts as stationary. |
| `harvesting` | Evaluated only when `ConsiderHarvestingAsStationary=no`. A unit that is harvesting, or an infantry in the shovel sequence. A harvester driving to ore is neither harvesting nor stationary. |
| `drain` | The object is being drained by a `DrainWeapon`. |
| `inrange` / `outofrange` | The object has a target and its distance to it is at most (`inrange`) or at least (`outofrange`) the range. The range is `DiscardOn.RangeOverride`, or the range of the weapon it would use, with range modifiers. |
| `firing` | Counts each shot, including spawner and drain shots, after the delayed-fire gate. Discards when the count reaches `DiscardOn.Firing.Count`. |
| `receiveddamage` | Counts each hit with positive final damage from a house allowed by `DiscardOn.ReceivedDamage.AffectsHouse`. Discards at `DiscardOn.ReceivedDamage.Count`. |
| `selling` / `undeploying` | A building in the selling mission. It is `undeploying` when it has an archive target (it turns into a unit), otherwise `selling`. |
| `invokerdie` | The invoker was destroyed. |
| `ammo` | Ammo is within the inclusive bounds; `-1` ignores a bound. |
| `health` | Health percentage is above `AbovePercent` and at most `BelowPercent`. |
| `mission` | The current mission is in `DiscardOn.Missions` (or `AIMissions` for objects of a computer-controlled house, when set). |
| `landtype` | The land type of the object's cell is listed. |
| `sequence` | Infantry only. With `Sequences.Immediate=yes`, while a listed sequence plays; otherwise once a listed sequence has finished and a different one starts. |
| `ownerchange` | Set when the owner changes. When both direction flags are on every change counts; when only one is on, only a change between a human-controlled and a computer-controlled house in that direction counts; when both are off none does. With `IgnoreRevertOnExit` the change caused by a passenger reverting to its own house on leaving a transport is ignored. A change while the object sits in a transport is also ignored unless it is that revert. |

- `firing` and `receiveddamage` only flag the instance. The discard takes place at the object's next update, so the shot or hit that reaches the count is still affected by the effect. A `DiscardOn=firing` firepower effect therefore boosts exactly the first shot.
- `ExpireWeapon` fires in these cases, selected by `TriggerOn`: `expire` (the duration ran out; effects whose delay is not negative never expire), `remove` (taken off by `RemoveTypes` or `RemoveGroups`), `death` (damage kills the object), `discard` (any discard condition, including `entry`). An effect dropped because the object changed type fires as `expire`. An effect dropped because the new type does not allow it after a deploy fires nothing.
- The weapon detonates at the object's position with the object as the target. Its owner is the object and its house, or with `UseInvokerAsOwner` the invoker (if still alive) and its house; if the invoker is gone only the invoker house is used. Weapons are detonated after the object's update loop finishes, so they never run while the list is being changed.
- `ExpireWeapon.CumulativeOnlyOnce` fires once for a cumulative type: on `expire` after the last active instance ended, on `death` once per type. The code appears to skip it for `remove` and `discard` (open question 8).

### Stat modifiers

- The multipliers of all active instances multiply. `FirepowerMultiplier`, `SpeedMultiplier`, `ROFMultiplier`, `Cloakable`, `ForceDecloak`, `DisableWeapons`, `Unkillable` and the non-conditional `ArmorMultiplier` are folded into one record per object when an instance attaches, ends or goes inactive. Inactive instances (delay, or offline with `Powered`) contribute nothing.
- Firepower: the weapon's damage is multiplied when it fires and when damage is estimated for targeting, truncated, together with the house and veteran bonuses.
- Armor: incoming damage is divided by the product, truncated. A multiplier of 2.0 halves damage. The code does not guard `0` or negative values (open question 17).
- Conditional armor multipliers (any of `AllowWarheads`, `DisallowWarheads`, `Chance`, `Delay`, `AffectsHouse`, `HitAnim` set) are evaluated per hit. Each active one is skipped when its `Delay` cooldown is running, when the roll against `Chance` fails (the random number is drawn for each such effect on every evaluation, even at `Chance=1.0`), when the attacker's house is not allowed by `AffectsHouse`, or when the warhead is disallowed or not in a non-empty allow list. A multiplier that applies on a real hit starts its `Delay` cooldown and plays one random `HitAnim` on the object. The same evaluation also runs when a damage figure is estimated for targeting, without cooldown or animation.
- Speed: the foot object's current speed is multiplied and truncated. Buildings have no speed.
- ROF: the weapon's `ROF` value is multiplied when the rearm delay is computed, so `0.5` halves the delay. A rearm already running is not changed when the effect ends. With `ROFMultiplier.ApplyOnCurrentTimer=yes`, when an instance first becomes active the remaining time of the running rearm timer (and the turret charge delay, if the last rearm was a full delay) is multiplied once. A multiplier of 0 or less is not applied to the running timer.
- Cloak: `Cloakable` adds the cloak ability while active, including the ready-to-cloak and should-not-be-cloaked tests. `ForceDecloak` makes the object never ready to cloak, forces the should-not-be-cloaked answer, and uncloaks a cloaked object when it takes effect.
- Weapon range: for each active instance whose allow and disallow lists permit the weapon, the range is multiplied by `WeaponRange.Multiplier` (minimum 0); `ExtraRange` values are summed, converted to leptons and added after all multipliers; the result is at least 0. The veteran range bonus applies first. The modified range is used in all range checks, including `DiscardOn` range. An open-topped transport with `OpenTopped.UseTransportRangeModifiers` applies the transport's effects to a passenger's weapon.
- Critical hit: for each active instance whose warhead lists permit the warhead, the chance is multiplied by `Crit.Multiplier` (minimum 0) and the `Crit.ExtraChance` values are summed and added.
- `Powered=yes`: an instance goes inactive while the object is deactivated, under EMP, or, for a building, not powered. The duration keeps counting. Going inactive or active recalculates the stat record and the tint.

### Animation and tint

- An instance creates its animation attached to the object, at the object's location, looping indefinitely. Its owner house is the object's owner, or with `UseInvokerAsOwner` the invoker house and invoker. The animation is created only while the instance is active, so it does not exist during a delay.
- The animation is removed (and comes back when the condition ends) while: the object is cloaked or cloaking and the AnimType has `DetachOnCloak=yes`; the object is burrowed in a tunnel; the instance is offline and `OfflineAction` is `Hides`; the object is held by a temporal weapon and `TemporalAction` is `Hides`; or another listed type from `HideIfAttachedWith` is on the object. `Temporal` marks the animation as frozen like a temporal victim, `Paused` pauses it, `PausedTemporal` does both, `None` leaves it running.
- `Animation.DrawOffset<n>` entries are added together for every entry whose `RequiredTypes` are all present (an empty list always counts). The offsets update when instances attach, end or change animation state. Phobos notes that hiding by `HideIfAttachedWith` may not refresh the offsets.
- A `Cumulative=yes` type with `CumulativeAnimations` shows one animation, held by one instance, instead of one per instance. The animation type is the entry at index `count - 1`, or `count / CountIncrement` when `CountIncrement` is above 1, where `count` is the number of active instances; counts past the end use the last entry. When the holding instance ends, another instance of the type (the one with the longest remaining time) takes over. If both `Animation` and `CumulativeAnimations` are set on a cumulative type, `CumulativeAnimations` is used.
- Tint is enabled when `Tint.Color` is not `0,0,0` or `Tint.Intensity` is not 0. A non-cumulative type tints once per instance. For a `Cumulative=yes` type with `Tint.Cumulative=no` only one instance of the type tints. With `Tint.Cumulative=yes`, colors of instances of the type add and intensities add. Colors from different instances are combined with a bitwise OR of the packed color, and intensities are summed and scaled by 1000 to integers. `VisibleToHouses` is evaluated against the viewing player: owner, allies and enemies are honoured; `neutral` has no effect.
- `LaserTrail.Type` creates a laser trail on the object when the instance is created and removes it when the instance is destroyed, including while the instance waits in a delay.

### Weapons and damage

- Weapon firing filters. A weapon with any of `RequiredTypes`, `DisallowedTypes`, `RequiredGroups`, `DisallowedGroups` checks the target (or the firer with `CheckOnFirer`). Checks run in this order and the first failure blocks the weapon: disallowed types, disallowed groups, required types, required groups. A group matches every effect type whose `Groups` contains the name.
- A disallowed set blocks when any member is present (`Disallowed.Any=yes`, the default) or when all are present (`no`). A required set passes when any member is present (`Required.Any=yes`) or all are (`no`, the default).
- Only active instances count. For a `Cumulative=yes` type the number of active instances must also lie within the min and max for that position; other types ignore the counts. For groups, the position is the order of the type in an internal list that Phobos builds from a pointer-ordered set (open question 13).
- `Required.Houses` and `Disallowed.Houses` compare the firer's house with the invoker house of each instance. `IgnoreFromSameSource` skips instances whose invoker and source match the firer and the weapon's warhead; `SameSourceOnly` keeps only those. Both need a known firer.
- The filter is applied wherever Phobos applies weapon target restrictions: firing, weapon choice (primary or secondary), target evaluation, wall-attack choice, and the projectile callbacks that pick targets for shrapnel and detonation. It is skipped for weapons with `SkipWeaponPicking=yes`.
- `RevengeWeapon`: when the object is killed by damage from a techno, the weapon of every active instance (whose `AffectsHouse` allows the killer's house) detonates on the killer, owned by the object, or by the invoker with `UseInvokerAsOwner`. Warheads can suppress revenge weapons with the Phobos revenge suppression keys.
- `ReflectDamage`: when the object takes damage above 0 from a warhead that is not already a reflected hit, each active instance not on cooldown that wins its `Chance` roll and allows the attacker's house deals damage back to the attacker. The damage is `Override` if set, otherwise the damage received times `Multiplier`. It uses `ReflectDamage.Warhead` (default `C4Warhead`) as ordinary damage, or as a full detonation at the attacker with `Warhead.Detonate=yes`. The reflected hit is marked on the reflect warhead so it cannot be reflected again. `Delay` starts a per-instance cooldown after each reflection. A warhead can set `SuppressReflectDamage`, optionally limited to listed types or groups. The code takes the attacker from the damage call without a null check (open question 12).
- `DisableWeapons`: every weapon fire check returns "still reloading" while any active instance has it, except for an object held by a locomotor warhead. Target evaluation ignores the flag, so the object keeps its target. With `OpenTopped.CheckTransportDisableWeapons`, passengers see their targets as out of range while the transport's weapons are disabled (the check includes the Ares weapon-disable timer when Ares is present).
- `Unkillable`: damage that would kill the object is cancelled and its health is set to 1. This covers damage that goes through the damage routine from a warhead whose house rules allow it to hurt the object. A death that does not pass through the damage routine is not prevented.

### Ownership and removal

- Removal by warhead or weapon (`RemoveTypes`, `RemoveGroups`) takes every instance of a type regardless of source or house. `RemoveGroups` removes every type present on the object that shares any listed group. `CumulativeRemoveMinCounts` (by position) leaves the type alone while fewer active instances exist; `CumulativeRemoveMaxCounts` removes at most that many instances, with `0` or less meaning all. Removal fires `remove` weapons and, for types with `RecreationDelays` of 0 or more, resets the instance instead of deleting it.
- An owner change (capture, mind control, `ChangeOwner`) keeps all effects and their invoker house. `DiscardOn=ownerchange` removes the ones that ask for it. If the new country has `AttachEffect.AttachOnOwnerChange` (or the `[General]` default) the new country's list is attached with the object as invoker. Effects of the previous country stay unless they discard on owner change.
- Entering a transport or structure freezes the effects (the update is skipped) and discards those with `DiscardOn=entry`.
- Deploy, undeploy and type conversion: effects with `AllowTransfer` (or `AllowTransfer.Convert` for conversion) move to the new object. Each is re-checked against the new type's `AffectsTarget`, `AffectTypes` and `IgnoreTypes`, and dropped without a weapon if it fails. If the new object already holds the type, a non-cumulative instance keeps the longer remaining time. A cumulative instance whose source already has an instance on the new object is merged (longer time kept) when the type is at its cap and dropped otherwise; with no same-source instance it is added without a cap check. An instance that is added keeps its remaining time, counters and cooldown timers. The new type then attaches its own list.
- When the invoker is destroyed its pointer is cleared, `invokerdie` effects are discarded, and `UseInvokerAsOwner` falls back to the invoker house.
- When the object is destroyed all its instances and animations are deleted. Only `death` expiry weapons fire.

### Determinism

- Random numbers come from the scenario's synchronised generator: conditional armor multiplier `Chance` (per effect, per evaluation, including damage estimates), `ReflectDamage.Chance`, and the `HitAnim` pick (a draw even with one entry). Armor evaluation for estimates also draws when the target has a conditional armor multiplier, and also when `Duration.ApplyArmorMultOnTarget` computes a duration.
- All timers are frame counts. The once-per-frame discard cache keys on the current frame number.
- Phobos keeps its group lookup in an unordered map of pointer-ordered sets and warns not to iterate it. A port must use declaration order wherever order can change the result.

## What stock YR does

Stock YR has no general attached effect. It has separate fixed states: Iron Curtain and Force Shield (timer, invulnerable), Berserk, veteran bonuses, crate bonuses, EMP and temporal. Each has its own timer and no INI-defined modifiers. Rate of fire, speed, firepower and armor each have only the house bias, per-object bias and veteran multipliers.

The OpenYR engine today also has the Ares-style effect (`AttachEffectTypeClass`, `AttachedEffectsClass`). See the table at the top of Behaviour.

## Where it hooks in OpenYR

Existing pieces:

- `code/attacheffect.h`, `code/attacheffect.cpp`: `AttachEffectTypeClass` (the Ares settings struct embedded in `WarheadTypeClass` and `TechnoTypeClass` as `AttachEffect`) and `AttachedEffectsClass` (one per `TechnoClass`, member `AttachedEffects`, with `Attach`, `AI`, `Limbo`, `Clear`, `Detach`, `Serialize`, `Compute_CRC` and the multiplier getters).
- `code/techno.cpp` call sites: `TechnoClass::AI` (`AttachedEffects.AI`), `TechnoClass::Limbo` (`AttachedEffects.Limbo`), `TechnoClass::Take_Damage` (Ares-style attach, and the armor divisor), `TechnoClass::Fire_At` (firepower), `TechnoClass::Rearm_Delay` (ROF), `TechnoClass::Is_Allowed_To_Recloak` (cloak), `TechnoClass::Detach`, `Serialize`, `Compute_CRC`; `FootClass::Current_Speed` in `code/foot.cpp` (speed).
- Engine facts the feature needs and already has: `IsForceShielded` and `Is_Iron_Curtained`, `IsBeingWarpedOut`, `IsDeactivated`, `DrainTarget`/`DrainingMe`, `BuildingClass::Is_Powered_On`, `DamageSourceHouse` (`code/combat.h`), `AnimClass::Attach_To` and `Loops`, `Fire_Death_Weapon` (the pattern for detonating a weapon at a point: `Create_Bullet`, `Detonate`), `TechnoClass::Captured` (owner change), `WarheadTypeClass::Find_Or_Make` and `RulesClass::Do_WarheadTypes` (the pattern for a named type list), `SaveVersionInfo::REVISION` in `code/savever.h`.
- Not present yet: the named effect type class and list, the discard framework, delay lists, per-instance state beyond the Ares record, tint, laser trails, EMP state (`specs/emp.md`), the Phobos critical-hit and revenge-weapon systems, a weapon range accessor that accepts modifiers (`TechnoClass::Weapon_Range` returns `weapon->Range`), and any country-level or weapon-level attach.

Build order, smallest first:

1. Type registry. Read `[AttachEffectTypes]` in `RulesClass` next to `Do_WarheadTypes` and the type sections after all type lists exist. Store types in declaration order with an index; give a listed name with no section the defaults. Add `Serialize` and a CRC contribution.
2. Self-attached effects. Parse the TechnoType list keys; create instances in the object's constructor path; tick them in `AttachedEffectsClass::AI`; support `Duration`, the delay lists and the stat multipliers by feeding them into the four existing call sites.
3. Warhead attach and remove at detonation, before damage, with the target filters of the warhead (see `armor-types.md` for `EffectsRequireVerses`). The natural place is the per-object loop in `Explosion_Damage` (`code/combat.cpp`) and the bullet detonation in `code/bullet.cpp`. The Ares-style attach in `Take_Damage` stays as it is.
4. Weapon attach in `TechnoClass::Fire_At`.
5. Stacking: `Cumulative`, caps, refresh rules, groups.
6. Discard conditions and `ExpireWeapon`, including the `Limbo`, death, `Captured` and deploy hooks.
7. Animation (including `CumulativeAnimations`, draw offsets, temporal and offline actions) and tint.
8. Weapon firing filters in `Can_Fire`, `What_Weapon_Should_I_Use` and `Evaluate_Object`.
9. `WeaponRange.*`, `Crit.*`, `Cloakable`/`ForceDecloak`, `DisableWeapons`, `Unkillable`, `Powered`, `ReflectDamage`, `RevengeWeapon`. The last three groups depend on other specs.
10. Country lists, owner change, and `AllowTransfer` for deploy (`UnitClass` creating a `BuildingClass` in `code/unit.cpp`) and undeploy (`code/building.cpp`).
11. Map trigger event 606, then `LaserTrail.Type` when laser trails exist.

## Saved state

Per object (the existing `AttachedEffects` member, extended or paralleled):

- For each instance: type index, remaining duration, stored duration override, delay, current delay, initial delay, recreation delay, invoker (object pointer), invoker house, source (warhead or TechnoType pointer), animation pointer, flags (anim hidden, in tunnel, under temporal, online, cloaked, initialised, refresh pending, discard pending, recalculate pending, update-anim pending, cumulative-animation holder, self-owned), last-discard-check frame and value, last powered state, last sequence, firing count, received-damage count, armor-multiplier cooldown timer, reflect cooldown timer, laser trail pointer.
- The folded stat record (multipliers and flags) can be rebuilt from the instances on load; Phobos saves it.
- Count of instances that name this object as invoker (a fast path for pointer cleanup; optional).

Global and per house:

- The effect type definitions with the rules (the engine saves type data with the type, as `WarheadTypeClass::Serialize` does for the Ares record). The group map is rebuilt from the types on load.
- Nothing per house. Country lists are rules data.

Network checksum: the number of instances, and for each its type index, remaining duration, delays, counters (firing, received damage), cooldown timers, cumulative-animation holder, and the folded multipliers. Random draws are in the shared generator and need no extra field. Raise `SaveVersionInfo::REVISION` when the saved layout changes, per `docs/SAVE-FORMAT.md`.

## Open questions

Decisions for the owner:

1. Extend `code/attacheffect.cpp` or live beside it. The names collide: Phobos calls its type `AttachEffectTypeClass`, as does the engine's Ares struct. One option is to extend: keep `AttachedEffectsClass` as the single per-object container and tick, add a kind tag to its instances, and give the Phobos type a new name. This keeps one multiplier product, one save section and one set of hook call sites. The other is to live beside: new files for the type registry and the instance class, leaving the Ares record untouched, and have `AttachedEffectsClass` and the new list both feed the multiplier getters. This isolates the Ares behaviour that mods such as Mental Omega rely on. Folding the Ares keys into implicit Phobos types is not safe, because refresh identity, attach time, cloak-hiding and the Verses rule all differ. A workable split is to keep one container and one tick, put the Phobos type and instance code in new files, and rename the Ares struct in a separate mechanical change.
2. Whether to copy Phobos quirks that look like bugs (items 4 to 14) or fix them. Each needs a classification: preserved, fixed or intentionally changed.
3. `[General]` spelling: the code reads `AttachEffects.AttachOnOwnerChange`, the docs print `AttachEffect.AttachOnOwnerChange`. A mod that follows the docs gets no effect in Phobos. Reading both would change what such a mod does here.
4. Whether to expose Phobos' interop exports (`AE_Attach`, `AE_Detach`, `AE_DetachByGroups`) in some plugin form. The engine has no plugin interface, so this is likely out of scope.

Unclear or odd in Phobos (from reading the code; each needs a test against real Phobos):

5. `InitialDelays`: the docs say the countdown pauses while a discard condition holds; the code decrements it every update and only pauses the delay after an effect ends and the recreation delay.
6. `DiscardOn.Health.BelowPercent` and `AbovePercent` default to -1 and are passed as the bounds of a check that requires health at most `BelowPercent`. With `DiscardOn=health` and only `AbovePercent` set, health can never be at most -1, so the effect is never discarded, which contradicts "-1 ignores the bound". See Test 11.
7. Effects with a delay of 0 or more (the default for type- and country-attached effects) never count as expired, so `ExpireWeapon` with `expire` never fires for them. Set `AttachEffect.Delays` negative to get expiry.
8. `ExpireWeapon.CumulativeOnlyOnce`: for `remove` and `discard` the instance being ended still counts as active when the check runs, so the weapon appears never to fire. It works for `expire` and `death`.
9. `RemoveGroups` builds a list with one entry per instance on the object. With cumulative types and `CumulativeRemoveMaxCounts` the per-entry limit is applied once per instance, so more than the limit can be removed, and the positional min/max lists no longer match the groups.
10. `CumulativeRefreshSameSourceOnly=yes` (the default) plus a full type cap reached by other sources means a new application does nothing at all.
11. A refresh without an override restores the override stored in the old instance. A finite new duration can shorten an indefinite instance of the same type.
12. `ReflectDamage` dereferences the damage source without a null check; damage with no attacker (radiation, some superweapons) may crash Phobos.
13. `RequiredGroups`/`DisallowedGroups` min/max positions follow a pointer-ordered set, so the order is unspecified across builds. Group lookup should use declaration order here.
14. A type in `[AttachEffectTypes]` with no section leaves the tint object null in Phobos and the first use dereferences it. Here an empty section should behave as all defaults, or be rejected at load with a log line.
15. Phobos tests `WeaponRange.Multiplier` against 0.0 to decide whether an effect modifies range. The default 1.0 never equals 0.0, so every effect counts as a range modifier. This costs time and does not change results.
16. Dependent systems are not specified here: the Phobos critical-hit chance model (`Crit.*`), revenge weapons, laser trails and the `DetachOnCloak` AnimType key. The `Powered` key needs the EMP model.
17. `ArmorMultiplier` of 0 or negative divides by zero or flips the sign of damage. The Phobos docs do not define it; the port should clamp or reject it. Whether healing (negative damage) is divided is also unstated.
18. The exact meaning of the "immobilised" flag that makes Phobos skip the effect update was not traced (open: whether it covers the whole time an object is held by a temporal weapon, which the engine marks with `IsBeingWarpedOut`). The Ares-style effect in OpenYR pauses on `IsBeingWarpedOut`; the same condition is the working assumption.
19. Conditional armor evaluation for damage estimates draws from the synchronised generator. If an estimate runs on only one machine (for example a cursor or sidebar test), clients can desync. The port should evaluate estimates without drawing.
20. Whether a warhead with `Damage=0` reaches detonation in stock YR was not traced; the Phobos attach path does not test the damage value.

## Test plan

Each test needs a build with the feature and a real Phobos run for comparison. Use a small test mod and a flat skirmish map where the human player owns every unit; force-fire with Ctrl-click, even on own units.

Common setup (`rulesmd.ini` additions):

```ini
[TestWH]                 ; plain damage, 100% against every armor
Verses=100%,100%,100%,100%,100%,100%,100%,100%,100%,100%,100%

[TestGun]                ; Shooter weapon, 100 damage per 60 frames
Damage=100
ROF=60
Range=10
Projectile=InvisibleLow
Warhead=TestWH

[MTNK]                   ; Grizzly is the Shooter
Primary=TestGun

[APOC]                   ; Apocalypse tank is the Dummy
Strength=300
Armor=none

[ApplyWH]                ; applier warhead, same Verses as TestWH
Verses=100%,100%,100%,100%,100%,100%,100%,100%,100%,100%,100%
AttachEffect.AttachTypes=AE_A

[ApplyGun]               ; Applier weapon, 1 damage per hit
Damage=1
ROF=60
Range=10
Projectile=InvisibleLow
Warhead=ApplyWH

[HTNK]                   ; Rhino is the Applier
Primary=ApplyGun

[AttachEffectTypes]
0=AE_A

[AE_A]
Duration=900
ArmorMultiplier=2.0
```

List the new warheads under `[Warheads]` and the new weapons under `[WeaponTypes]`. Counts below are shots of `TestGun` until the Dummy dies. Each application of `ApplyGun` costs the Dummy 1 health (a hit is never below 1 damage).

1. Warhead armor effect (differs from stock). Force-fire the Applier once at Dummy 1. Then force-fire the Shooter at Dummy 1 until it dies and count shots. Then do the same at Dummy 2, which was not hit. Expected: Dummy 1 dies on the 6th shot (50 damage per shot against 299 health); Dummy 2 on the 3rd. Hit Dummy 3 once with the Applier, wait 900 frames, then shoot it: expected 3 shots, since the effect has ended.
2. Refresh versus stacking. Add `AE_B` (`Cumulative=yes`, `Cumulative.MaxCount=2`, `Duration=900`, `ArmorMultiplier=2.0`) and a second applier warhead that attaches it. Hit one Dummy three times with the `AE_A` Applier and another three times with the `AE_B` Applier, then shoot each. Expected: the `AE_A` Dummy takes 6 shots (one instance, refreshed); the `AE_B` Dummy takes 12 shots (two instances, 25 damage per shot, the third hit only refreshes). Edge: set `Cumulative.MaxCount=0` and repeat: the Dummy takes 3 shots.
3. Next-shot effect. Give `[MTNK]` `AttachEffect.AttachTypes=AE_C` with `[AE_C]` `Duration=-1`, `FirepowerMultiplier=3.0`, `DiscardOn=firing`. Shoot Dummy 1, then Dummy 2. Expected: Dummy 1 dies on the first shot; Dummy 2 needs 3 shots. Edge: add `AttachEffect.RecreationDelays=300` to `[MTNK]`; after a kill, wait 300 frames and shoot a new Dummy: expected one shot again.
4. Expiry and death weapons. Add `[BoomGun]` (`Damage=100`, `Warhead=BoomWH` with `Verses` as above, `CellSpread=1.5`, `PercentAtMax=1.0`, same projectile). `AE_D1`: `Duration=300`, `ExpireWeapon=BoomGun`. `AE_D2`: `Duration=-1`, `ExpireWeapon=BoomGun`, `ExpireWeapon.TriggerOn=death`. Attach `AE_D1` to Dummy 1 and `AE_D2` to Dummy 2 (one applier warhead per effect), each with a Witness (another Apocalypse) one cell away. Wait 300 frames: expected Dummy 1 takes 100 damage and the Witness beside it takes 100. Kill Dummy 2 with the Shooter: expected the Witness beside it takes 100 at the moment of death, and Dummy 2 takes nothing at 300 frames.
5. Weapon firing filter. Add `[TestGun2]` (as `TestGun`, plus `AttachEffect.RequiredTypes=AE_E`), give `[MTNK]` `Primary=TestGun2`, and `AE_E` (`Duration=900`, no modifiers) attached by the Applier. Force-fire the Shooter at an unmarked Dummy: expected no shot for at least 10 seconds. Mark the Dummy with the Applier: expected the Shooter fires within about 60 frames, and stops about 900 frames after the mark. Swap to `AttachEffect.DisallowedTypes=AE_E`: expected the reverse.
6. Remove by group. `AE_F` (`Groups=BUFF`, `Duration=-1`, `ArmorMultiplier=2.0`) and `AE_G` (`Duration=-1`, `ArmorMultiplier=2.0`), both attached by one Applier hit. A second warhead has `AttachEffect.RemoveGroups=BUFF`. Expected: a Dummy hit only by the first warhead dies in 12 shots (25 per shot); a Dummy hit by the first and then the second dies in 6 shots (`AE_G` stays).
7. Discard on entry. Give two infantry types (for example Conscript and GI) the same self-attached tint effect (`Tint.Color=255,0,0`, `Tint.Intensity=0.3`, `Duration=-1`), one with `DiscardOn=entry`. Load both into a Flak Track and unload them. Expected: both are red before loading; afterwards the infantry with `DiscardOn=entry` is back to normal colour and the other is still red.
8. Discard on owner change. Give two buildings that an Engineer can capture (for example a Power Plant and a Tesla Reactor of a second house) the same tint effect, one with `DiscardOn=ownerchange`. Capture both. Expected: the tint stays only on the building without `DiscardOn=ownerchange`.
9. Type filters. `AE_H`: `Tint.Color=255,0,0`, `Duration=600`, `AffectTypes=HTNK`. Hit a Rhino and a Grizzly with the Applier carrying it. Expected: only the Rhino turns red for 600 frames. Change to `IgnoreTypes=HTNK` and repeat: only the Grizzly turns red.
10. Cloak ability. `AE_I`: `Duration=900`, `Cloakable=yes`. Hit a stopped Grizzly. Expected: within a few seconds it shows as cloaked for its owner, and becomes visible again about 900 frames later. Add `AE_J` with `ForceDecloak=yes` and apply both: expected the Grizzly stays visible while `AE_J` lasts.
11. Health bounds (code against docs). `AE_K`: tint, `Duration=-1`, `DiscardOn=health`, `DiscardOn.Health.AbovePercent=0`, `DiscardOn.Health.BelowPercent=0.5`, attached to a Dummy. Expected: red at full health; the tint goes out once two shots bring the Dummy to one third health. Variant: only `AbovePercent=0.5` set. The docs predict the effect is discarded at full health; the code reading predicts it stays red forever. Record which happens.
12. Multiplayer sync (two PCs). In a two-player skirmish give both sides units with an effect `ArmorMultiplier=2.0`, `ArmorMultiplier.Chance=0.5`, `ArmorMultiplier.HitAnim=<any anim>` and `ReflectDamage=yes`, `ReflectDamage.Chance=0.5`. Fight for five minutes. Expected: no desync dialog; the same units die on both screens.
