# EMP model and EMP.Threshold (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/restored/emp.html.

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `EMP.Duration` | rules, WarheadType | integer frames; negative heals | none (stock uses the damage value) |
| `EMP.Cap` | WarheadType | integer frames | `-1` |
| `ImmuneToEMP` | TechnoType | boolean | derived, see below |
| `EMP.Modifier` | TechnoType | float multiplier on positive durations | `1.0` (100%) |
| `EMP.Threshold` | TechnoType | `yes`, `no`, `inair`, or an integer frame count | `inair` for aircraft, `no` otherwise (see open questions) |
| `EMP.Sparkles` | TechnoType, and `[Warhead]` override | AnimationType | stock `EMPulseSparkles` |
| `EMPAIRecoverMission` | rules, `[CombatDamage]` | mission | `Hunt` |
| `EMPIMMUNE` | veteran or elite ability list | ability name | none |

## Behaviour

Every techno has a hidden EMP counter that falls by one each frame until it reaches zero. While it is above zero the object is disabled: it cannot take commands, move or attack.

How a hit changes the counter depends on `EMP.Duration` (D) and `EMP.Cap` (C) of the warhead:

| Case | Effect on the counter |
|---|---|
| D > 0, C > 0 | add D, but never above C |
| D > 0, C = 0 | add D with no limit |
| D > 0, C = -1 | set to D unless the counter is already higher |
| D < 0 (healing EMP), C > 0 | subtract |D|, and if still above C lower it to C |
| D < 0, C = 0 | the object reactivates at once |

The warhead's `Verses` entry for the target's armor decides whether the EMP affects it at all: 0% means no effect.

Immunity: `ImmuneToEMP` overrides the defaults. Defaults: buildings are immune unless they need power or provide a special function (radar, super weapon and similar); infantry are immune unless Cyborg; vehicles and aircraft are immune if Organic. A veteran or elite object gains immunity from the `EMPIMMUNE` ability.

`EMP.Modifier` scales positive durations for that type. For example 50% halves every EMP applied to it.

`EMP.Threshold` makes EMP destructive:

- A positive number (`yes` equals 1): the object is destroyed when its counter exceeds the number.
- A negative number: the object is destroyed only if it is airborne when the counter exceeds the absolute value (`inair` equals -1).
- `0` or `no`: never destroyed by EMP.
- Flying units default to `inair`.

`EMP.Sparkles` is the animation shown while disabled. `EMPAIRecoverMission` is the mission an AI unit takes when the counter reaches zero.

## What stock YR does

An EMP warhead (`EMEffect=yes`) creates an `EMPulseClass` at the hit cell with the warhead's `CellSpread` and a duration equal to the damage value. Objects in the radius that are not immune are stunned for that duration. Airborne aircraft crash. Buildings power off. A limpet mine is destroyed. `ImmuneToEMP` is the only per type immunity key; buildings default to immune when they are core defenders. A second pulse overwrites the stun with its own duration, so there is no stacking.

## Where it hooks in OpenYR

- `EMPulseClass::Create` in `code/empulse.cpp` is the stock effect: it sets `foot->StunDuration = Duration` for units, `building->StunDuration = Duration` for buildings, and calls `aircraft->Crash(source)` for flying aircraft. The Ares model replaces the plain assignment with the counter rules above, using `TechnoClass::StunDuration` (`code/techno.h`, saved and in the CRC) as the counter and ticking it where `TechnoClass::AI` decrements it (`code/techno.cpp`, the `if (StunDuration > 0)` block, which also turns power and locomotion back on).
- Duration source: `BulletClass::Detonate` (`code/bullet.cpp`) creates the pulse with the damage as `Strength`. `EMP.Duration` needs its own warhead field passed to the pulse (`code/warhead.cpp`, near `IsEMEffect`).
- Immunity: `TechnoTypeClass::Is_Immune_To_EMP()` (`code/techtype.cpp`) returns `IsImmuneToEMP.value_or(false)` and the building override (`code/builtype.cpp`) falls back to `IsCoreDefender`, so `ImmuneToEMP` is already an optional per type setting. The Ares defaults (building power, cyborg, organic) must be filled in where the optional is unset. Add `EMPIMMUNE` to `AbilityType` in `code/ability.hh` (it has stock abilities such as `ABILITY_GUARD_AREA` but no Ares ones) and to the `Has_Ability` path.
- Threshold: after the counter changes, compare against the threshold. Destroy with `Take_Damage` using the technos' full strength (compare how `AircraftClass::Crash` kills), or `Do_Destruction` for buildings. Flying detection: `In_Air()` on the object.
- Sparkles: `Rule->EMPulseSparkles` is used for the animation; add a per type and per warhead override.
- Disabled state: `TechnoClass::Can_Player_Fire`-style check (`StunDuration > 0 || IsDeactivated || ...`) already blocks actions; `drive.cpp` also tests `StunDuration`.
- AI recovery mission: assign in the `StunDuration == 0` block for AI houses.

## Open questions

1. The documentation gives `yes = 1`, `inair = -1`, `no = 0` and says "positive values destroy if EMP duration exceeds threshold", but does not say whether the comparison uses the counter or the single hit's duration, nor whether it is `>` or `>=`.
2. Whether the default for ground units is `no` or whether Ares applies `inair` to all. The page says the default is `inair` "for flying units".
3. Whether `EMP.Duration` replaces the damage-based duration for every EMP warhead, or only when it is set. The page says it is the primary duration value.
4. Whether the area effect (`EMPulseClass`, which stays alive for its duration and marks cells as under EMP) is kept alongside the per object counter. The stock engine applies the stun once when the pulse is created.
