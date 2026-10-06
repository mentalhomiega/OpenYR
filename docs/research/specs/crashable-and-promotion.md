# Crashable, CrashSpin and promotion keys (Ares)

Sources:
- Crashing: https://ares-developers.github.io/Ares-docs/new/crashableaircraft.html
- Promotion: https://ares-developers.github.io/Ares-docs/new/promotion.html
- Phobos `CrashSpin.Multiplier`: https://phobos.readthedocs.io/en/latest/Fixed-or-Improved-Logics.html (section "Customize crash spin multiplier")

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `Crashable` | rules, AircraftType | boolean | `yes` |
| `CrashSpin` | rules, AircraftType (not jumpjet vehicles) | boolean | `yes` |
| `CrashSpin.Multiplier` (Phobos) | TechnoType with `Locomotor=Fly` | float | `1.0` |
| `Promote.VeteranSound` | rules, TechnoType | sound | `[AudioVisual] UpgradeVeteranSound` |
| `Promote.EliteSound` | TechnoType | sound | `[AudioVisual] UpgradeEliteSound` |
| `Promote.VeteranFlash` | TechnoType | frames | `[AudioVisual] VeteranFlashTimer` |
| `Promote.EliteFlash` | TechnoType | frames | `[AudioVisual] EliteFlashTimer` |
| `EVA.VeteranPromoted` | TechnoType | EVA entry | the global one |
| `EVA.ElitePromoted` | TechnoType | EVA entry | the global one |
| `VeteranFlashTimer` | `[AudioVisual]` | frames | `0` |
| `Promote.IncludePassengers` | TechnoType | boolean | `no` |

## Aircraft crash

- `Crashable=no`: a destroyed aircraft explodes where it is and disappears. With `yes` it falls to the ground and crashes.
- `CrashSpin=no`: a crashing aircraft glides down instead of spinning uncontrollably. Jumpjet vehicles do not support it.
- Phobos `CrashSpin.Multiplier` scales the spin speed instead of turning it off.

## Promotion feedback

- When an object promotes, the sound, the flash time and the EVA line come from the type's `Promote.*` and `EVA.*` keys, each falling back to the global setting. Stock YR only flashed on elite; with Ares a veteran promotion can flash too, using `VeteranFlashTimer`.
- `Promote.IncludePassengers=yes` sets every trainable passenger of the transport to the transport's rank whenever the transport's rank changes. The passengers' own rank is discarded, so an elite passenger can drop to veteran in a manually controlled transport.
- The sound and EVA play only for the owning player's objects (observed in stock behaviour; the Ares page does not restate it).

## What stock YR does

Aircraft always crash. All types use `UpgradeVeteranSound`, `UpgradeEliteSound` and `EliteFlashTimer`. Only elite promotion flashes. There is no passenger inheritance.

## Where it hooks in OpenYR

Crash:
- `AircraftClass::Crash(TechnoClass * source)` (`code/aircraft.cpp`) starts the crash: it records the kill, sets `Strength = 0`, starts rocking and randomises the rock rates, then lets the aircraft fall. `Crashable=no` branches near the top: record the kill, spawn the explosion at the current coordinate and delete the aircraft instead of entering the fall. It is called from the damage path (`code/aircraft.cpp`, the `if (!Crash(source))` call) and from `EMPulseClass::Create` (`code/empulse.cpp`), so the key covers EMP deaths too.
- `CrashSpin` and the spin multiplier affect the per frame rotation values set in `Crash` (`RockingSidewaysPerFrame`, `RockingForwardsPerFrame`) and the fall update that applies them.
- Type read: `AircraftTypeClass` (`code/airctype.cpp`).

Promotion:
- The promotion check is in `TechnoClass::AI` (`code/techno.cpp`): it compares `CurrentRank` with the rank from `Veterancy.Is_Elite()` and `Is_Veteran()`, plays `Rule->UpgradeEliteSound` or `Rule->UpgradeVeteranSound` for the player's own objects, speaks `EVA_UnitPromoted`, and sets `FlashCount = Rule->EliteFlashTimer` on elite. Replace each global with the type's key (fallback to the global), and set `FlashCount` for veteran too from `VeteranFlashTimer` (add it to `RulesClass`, `code/rules.cpp`, next to `EliteFlashTimer`).
- Type read: `TechnoTypeClass` (`code/techtype.cpp`), next to the other sound reads.
- Passenger rank: the passengers are in `TechnoClass::Cargo` (`code/techno.h`); `Veterancy` is `VeterancyClass` (`code/veteran.cpp`, `code/veteran.h`). Set the passengers' veterancy where the rank change is detected.

## Open questions

1. Whether `Crashable=no` aircraft still grant kill credit and trigger death events (assume yes).
2. Whether `VeteranFlashTimer` is applied to the rank change from creation (starting rank) as well as to promotions. Stock `CurrentRank` starts at -1 and does not announce the first rank.
3. EVA keys are listed on the page but the default entry names are not.
