# Tunnel networks (Ares)

Source: https://ares-developers.github.io/Ares-docs/new/buildings/tunnels.html

Not implemented. This file records what the Ares tunnel network needs from our engine, so the work can be planned. The key rows are in `docs/research/ares-tags.md` (`Tunnel`, `[TunnelTypes]`, `Passengers`, `MaxSize`).

## What Ares documents

- `[TunnelTypes]` lists the networks. Each entry names a TunnelType section with `Passengers` (the most occupants, one slot per unit whatever its size, default 0) and `MaxSize` (the largest `Size` that may enter, default 0.0).
- `Tunnel=<TunnelType>` on a BuildingType makes the building an entrance to that network. Several building types may share one network.
- All entrances of a network share one list of occupants. A unit that enters one entrance can be ejected from any entrance of the same network at once; it does not travel between them.
- Entrances may not have weapons or turrets and may not be capturable or mind-controllable. Hover units have the same problems as with `UnitAbsorb=yes` buildings.
- Ejection does not check `MaxSize`.
- The occupants live while at least one entrance of that network owned by the same player stands. When the last one is destroyed, the occupants die and the player who destroyed it is credited with the kills. When the last one is sold, the occupants try to get out.
- Entrances play `EnterTransportSound` and `LeaveTransportSound` and show pips for the network's contents.
- The enter cursor appears only when the TunnelType has `Passengers` and `MaxSize` set.

## What we have

- `Tunnel` appears in our code only as the name of a land type (`code/const.cpp`, `code/rules.cpp`); nothing reads the building key or `[TunnelTypes]`.
- `UnitAbsorb=yes` / `InfantryAbsorb=yes` structures already take objects off the map into the building's own `Cargo`: `BuildingClass::Can_Absorb` (owner, kind and room under `Passengers`), `BuildingClass::Absorb` and the arrival paths in `code/unit.cpp` and `code/infantry.cpp`. `BuildingClass::Release_Passenger` and `BuildingClass::Place_Beside_Structure` put a passenger back beside the structure, and `BuildingClass::Do_MISSION_UNLOAD` runs the unload mission (0x44D880).
- `TechnoClass::Kill_Cargo` kills every passenger and credits the killer, which `BuildingClass::Do_Destruction` calls when a structure dies.
- `Bunker=yes` (the tank bunker) holds one vehicle that fights from inside (`BunkerLinkedItem`); it is a single-occupant link, not shared storage, and is not a good base for networks.
- `TechnoClass::Play_Transport_Sound` plays a type's `EnterTransportSound` / `LeaveTransportSound`. `Size` and `SizeLimit` are read as integers on every TechnoType; Ares gives `MaxSize` as a decimal, so the size check needs a decision on rounding.
- Pips come from the type's `PipScale` and count the object's own `Cargo`; a network would need them to count the shared list instead.

## What the work needs

1. Rules: a `TunnelTypeClass` (name, `Passengers`, `MaxSize`) read from `[TunnelTypes]`, saved with the other type lists, and `Tunnel=` on BuildingType resolved to it. A declared table fits, since the type has two keys.
2. Storage: one occupant list per network and house, held by the house (for example a map from TunnelType to a `FootClass` list on `HouseClass`), not by any one building. It is new saved state and needs its own Serialize, pointer fix-up on load and `Detach` handling, so the save revision goes up.
3. Entry: an entrance accepts a unit of its owner when the network list has room under `Passengers` and the unit's `Size` is at most `MaxSize`. The absorb arrival paths can be reused with the check and the target list swapped. The enter cursor follows the same check.
4. Exit: the unload mission of any entrance takes occupants from the shared list and places them with `Place_Beside_Structure`. The order of ejection is not documented (see below).
5. Loss: when an entrance dies, is sold or changes owner, count the house's remaining entrances of that network. With none left, a destroyed entrance kills the occupants through the `Kill_Cargo` logic with the destroyer as the source; a sold one ejects them.
6. Display: pips and the unload button of every entrance read the shared list.
7. Validation: log entrance types that also have weapons, a turret, `Capturable=yes` or are not immune to mind control, as Ares forbids them.
8. Tests: two entrances with a unit entering one and leaving the other; a full network refusing a unit; `MaxSize` refusing a large unit; the last entrance destroyed (occupants die, killer credited) and sold (occupants come out); a save and load with occupants inside.

## Open questions

The Ares page leaves these open; each needs a black-box run with Ares, or an INI key with a documented default.

- Whether a network is per house (the survival rule suggests it) or shared by allies.
- Whether the entrance's limit is the TunnelType's `MaxSize` or the building's own `SizeLimit`; the page names both.
- Which occupants leave first and through which cells, and whether one unload empties the network.
- What happens to the occupants when an entrance is captured or changes owner while other entrances of the old owner remain, and when the only entrance changes owner.
- Whether the computer players use tunnels at all.
- Whether entering or leaving uses the building's door cell, as a factory does, or any adjacent cell.

Mental Omega sets `Tunnel` on two building types and in a number of mission maps; until this is done those buildings behave as plain structures.
