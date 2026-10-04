# Chronoshift controls (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/new/chronoshift.html.

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `Chronoshift.Allow` | rules, TechnoType | boolean | `yes` |
| `Chronoshift.Crushable` | rules, TechnoType (not BuildingType) | boolean | `yes` |
| `Chronoshift.IsVehicle` | rules, BuildingType | boolean | `no` |
| `ChronoInfantryCrush` | rules, `[General]` | boolean | `yes` |
| `Chronosphere.ReconsiderBuildings` | rules, `[General]` (existing Ares key that `IsVehicle` needs) | boolean | `no` |

## Behaviour

- `Chronoshift.Allow=no`: the Chronosphere ignores the object when it picks units to move. It stays where it is and is not killed.
- `Chronoshift.Crushable=no`: when a chronoshifted unit lands on a cell holding this object, the arriving unit is destroyed and the object survives. Buildings are not covered by this key.
- `ChronoInfantryCrush=no`: a chronoshifted infantry unit dies on landing instead of destroying a tank under it.
- `Chronoshift.IsVehicle=yes` treats a deployed-vehicle building as a vehicle for Chronosphere selection. It only has effect with `Chronosphere.ReconsiderBuildings=yes`.

## What stock YR does

The Chronosphere picks every ground foot unit in the 3x3 cells around the source, kills organic non-teleporters, skips Iron Curtained units and vehicles in a war factory, and sets the rest down at the same offset from the target. A landing unit destroys whatever is on its landing cell (an infantryman destroys infantry on the exact spot only) and dies if something there is Iron Curtained.

## Where it hooks in OpenYR

All in `code/super.cpp`:

- `SuperClass::Chrono_Warp(Cell const &)` builds the `moving` and `killed` lists. `Chronoshift.Allow` is one more skip next to the existing Iron Curtain and weapons-factory skips. `Chronoshift.IsVehicle` would add buildings to the candidate loop, which today visits only `Is_Foot()` objects; moving a building is a new capability, so leave it until a mod needs it (large).
- `static Chrono_Shift(FootClass *, Coord, ...)` builds the `crushed` list and calls `Chrono_Kill` on each entry. `Chronoshift.Crushable` is a test per entry: if an entry is not crushable, call `Chrono_Kill(foot)` for the arriving unit and leave the entry alone. `ChronoInfantryCrush` changes the `foot->RTTI == RTTI_INFANTRY` branch.
- `TechnoTypeClass` read (`code/techtype.cpp`) and `RulesClass` general read (`code/rules.cpp`) for the new keys.
- The Chronosphere AI target scoring in `SuperClass` should also skip objects that are not allowed.

## Open questions

1. Whether `Chronoshift.Allow=no` on one object affects the units around it. Assume no.
2. Whether the key also covers the chrono weapon (Chrono Legionnaire style warhead) or only the super weapon. The page says "the Chronosphere".
3. Whether a unit destroyed by `Chronoshift.Crushable=no` counts as a kill for the victim's owner.
4. The default behaviour of infantry landing on a tank in stock YR; confirm before adding `ChronoInfantryCrush`, since the OpenYR code only crushes infantry on the exact spot.
