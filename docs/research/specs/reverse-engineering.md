# Reverse engineering (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/new/reverseengineerlogic.html.

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `ReverseEngineersVictims` | rules, BuildingType | boolean | `no` |
| `CanBeReversed` | rules, InfantryType or VehicleType | boolean | `yes` |
| `ReversedAs` | rules, InfantryType or VehicleType | TechnoType | none (the unit itself) |
| `Grinding` | rules, BuildingType (stock key, required) | boolean | `no` |

## Behaviour

- A building with both `Grinding=yes` and `ReverseEngineersVictims=yes` reverse engineers each infantry or vehicle it grinds. The owner gains the ability to build the victim's type, or the type named by the victim's `ReversedAs`.
- `CanBeReversed=no` on the victim stops that unit from granting anything.
- Reverse engineered types ignore `Prerequisite`, `StolenTech`, `TechLevel`, `RequiredHouses` and `ForbiddenHouses`. They still obey `BuildLimit`, theater prerequisites, `Factory` and `Naval`.
- Audio: the first time the owner reverses a unit type, EVA says `EVA_ReverseEngineeredInfantry` or `EVA_ReverseEngineeredVehicle`, then `EVA_NewTechnologyAcquired`. When a spy undoes it, the message is `EVA_TechnologyStolen`.
- The documentation warns of minor bugs in the feature.

## What stock YR does

The grinder (`Grinding=yes`) refunds credits and destroys the unit. There is no tech gain. Stock YR has tech stealing by spy infiltration (`StolenTech`) which unlocks specific buildings for the spy's house.

## Where it hooks in OpenYR

- Grinding: `BuildingClass::Grind(FootClass *)` in `code/building.cpp` (called from the enter handling when `Class->IsGrinding`) pays the refund and removes the unit. The reverse hook goes here, after the unit is accepted and before it is destroyed.
- Per house unlock state: the sets of "owned tech" are in `HouseClass` (`code/house.cpp`; see `IsSide0TechStolen` and `IsWarFactoryInfiltrated`, used in `BuildingClass::Spied_By`). Reverse engineered types need a per house set of `TechnoTypeClass` pointers (or a bit vector by RTTI and heap id) that the buildable test `HouseClass::Can_Build` (or the function that checks prerequisites in `code/house.cpp`) consults, bypassing the listed prerequisite checks but keeping `BuildLimit`.
- The set must be serialised and in the CRC.
- Sidebar refresh: the house recalculation flag `IsRecalcNeeded` is set by the infiltration code and reused here.
- Type read: `TechnoTypeClass` (`code/techtype.cpp`) for `CanBeReversed` and `ReversedAs`; `BuildingTypeClass` (`code/builtype.cpp`) for `ReverseEngineersVictims`.
- EVA: `Speak_Eva` (used throughout `code/building.cpp`); the entries must exist in the mod's EVA data.
- Spy undo: infiltrating the reversing building removes the set (`BuildingClass::Spied_By`).

## Open questions

1. How a spy "undoes" it: whether it removes all reverse engineered types of the victim's house or just the latest.
2. Whether a unit in the set is lost when the reversing building is sold or destroyed.
3. Whether `ReversedAs` can name a type that itself has `ReversedAs`.
