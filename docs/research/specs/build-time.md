# BuildTime.* (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/new/buildtime.html.

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `BuildTime.MultipleFactory` | rules, TechnoType | float factor | `[General] MultipleFactory` |
| `BuildTime.Speed` | TechnoType | float, minutes for 1000 credits | `[General] BuildSpeed` |
| `BuildTime.Cost` | TechnoType | integer credits | the type's `Cost` |
| `BuildTime.LowPowerPenalty` | TechnoType | float | the matching `[General]` setting |
| `BuildTime.MinLowPower` | TechnoType | float | the matching `[General]` setting |
| `BuildTime.MaxLowPower` | TechnoType | float | the matching `[General]` setting |

## Behaviour

- `BuildTime.MultipleFactory` is the factor applied once for each extra factory of the same kind that the owner has. With the global default 0.8 and three factories, build time is multiplied by 0.8 twice; this key replaces 0.8 for the type.
- `BuildTime.Speed` and `BuildTime.Cost` replace the two inputs of the stock build time formula. Build time is `BuildTime.Speed` minutes times `BuildTime.Cost` / 1000.
- The three low power keys replace the global low power production limits for this type.
- Walls: when `BuildTime.Speed` is set explicitly on a wall, `WallBuildSpeedCoefficient` is not applied and the value is final.

## What stock YR does

All of these values come from `[General]` and apply to every type. `MultipleFactory` (default 0.8, capped by `MultipleFactoryCap`) is applied per extra factory to any type.

## Where it hooks in OpenYR

- `TechnoClass::Time_To_Build` in `code/techno.cpp` is the whole formula: it takes `Class_Of()->Time_To_Build()`, multiplies by `House->BuildSpeedBias`, divides by a power fraction clamped to 0.5 to 1.0 with a floor of `Rule->MinLowPowerProductionSpeed`, then multiplies by `Rule->MultipleFactory` once per extra factory (`House->Factory_Count(RTTI) - 1`, capped by `MultipleFactoryCap`), then by `Rule->WallBuildSpeedCoefficient` for walls. `BuildTime.MultipleFactory` replaces `Rule->MultipleFactory` in that loop, and the low power keys replace the clamp constants (0.75 and 0.5 are literals today and `MinLowPowerProductionSpeed` is the global floor). `val` is an `int` and each step multiplies it by a `double`, so the result is truncated after every step; keep that, or build times will differ from stock by a frame or two.
- `TechnoTypeClass::Time_To_Build` (`code/techtype.cpp`) holds the base time computed from cost and `BuildSpeed`. `BuildTime.Speed` and `BuildTime.Cost` change its inputs.
- `FactoryClass::Build_Rate` (`code/factory.cpp`) turns the result into a per step delay and clamps it to 1..255.
- Per type read: `TechnoTypeClass` INI read in `code/techtype.cpp`. Globals are read in `code/rules.cpp` (`MultipleFactory`, `MultipleFactoryCap`).
- The cost used for build time only matters here; the money charged still uses `Cost_Of`.

## Open questions

1. Whether `MultipleFactoryCap` applies when `BuildTime.MultipleFactory` is set.
2. Whether the factory count used for the multiple factory loop counts factories of the same RTTI or the same exact type. Stock counts by `RTTI` (`Factory_Count(RTTI)`).
3. Rounding of the intermediate time with a custom `BuildTime.Speed`.
