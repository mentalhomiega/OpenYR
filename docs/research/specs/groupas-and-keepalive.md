# GroupAs and KeepAlive (Ares)

Sources:
- GroupAs: https://ares-developers.github.io/Ares-docs/new/typeselect.html
- KeepAlive: https://ares-developers.github.io/Ares-docs/new/keepalive.html

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `GroupAs` | rules, TechnoType | string, at most 30 characters | the type's own ID |
| `TypeSelectUseDeploy` | rules, `[General]` | boolean | `yes` |
| `KeepAlive` | rules, TechnoType | boolean | buildings: `yes` when `Insignificant=no` and `DontScore=no`; infantry, vehicles, aircraft: `no` |
| `BaseUnit` | rules, `[General]` (stock) | list of vehicle types | none |

## GroupAs

- Type select (the command that selects every visible unit of the same type as the clicked one, usually by double click or a hotkey) compares group names instead of type IDs. Setting `GroupAs=DOG` on every dog type makes one click select all of them.
- Works across infantry, vehicles, aircraft and buildings. Types with no `GroupAs` form a group of their own, named by their ID.
- Do not use an existing type ID as a group name; the type with that ID would join the group.
- `TypeSelectUseDeploy=yes` puts a type and the type it deploys into (`DeploysInto`, `UndeploysInto`) in the same group automatically.
- No side effects when types are changed in maps.

## KeepAlive

- In a Short Game, a player loses when they own no object whose `KeepAlive` is true. This replaces the stock test of "owns a building" for Short Game.
- A vehicle listed in `BaseUnit` always keeps the player alive.
- `KeepAlive` does not depend on `DontScore` or `Insignificant`, which still control scoring and the other lose conditions.

## What stock YR does

Type select groups by type ID (and deploy partner). Short Game ends a player when they have no buildings and no `BaseUnit` vehicle. Insignificant buildings are handled by the building counters.

## Where it hooks in OpenYR

GroupAs:
- The type select command is bound as `TypeSelect` in `code/tab.cpp` (the command name table); the selection routine itself was not located by name during this research. Search the keyboard command dispatch (`code/_command.cpp`, `code/_keyboar.cpp`) and `DisplayClass`/`TacticalClass` selection code for the loop that compares `Class_Of()` between objects, and replace that comparison with a group key from the type. Store the group key (string or interned ID) on `TechnoTypeClass` (`code/techtype.cpp`).

KeepAlive:
- `HouseClass::AI` in `code/house.cpp` has the multiplayer defeat test: with `Session.Options.ShortGame` it requires `!CurBuildings && Count_Owned(UQuantity, Rule->BaseUnit) == 0`. `KeepAlive` needs a per house count of objects with `KeepAlive=yes`, maintained where the other counters (`CurBuildings`, `CurUnits`, `CurInfantry`, `CurAircraft`) change, or computed on demand at that test (it runs once per house per frame, so a scan of the house's objects every few frames is acceptable). Counters are serialised and in the CRC, so a new counter needs both.
- The type flag is read in `TechnoTypeClass` (`code/techtype.cpp`).

## Open questions

1. Whether `KeepAlive` changes the non-short game defeat condition (the page speaks only of Short Games).
2. Whether the group name is case sensitive.
3. Whether Insignificant buildings given `KeepAlive=yes` appear in the `CurBuildings` counter used for the other checks.
