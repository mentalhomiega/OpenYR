# Mind-controlled units entering things, 2026-10-04

Branch `cloud/mind-control-enter-2026-10-04`, based on `origin/yr` at `71b4cf07`.

The owner's answer in `CLOUD_BRIEFING.md` is that a mind-controlled unit cannot enter a transport or a structure, except a Bio Reactor. This branch makes the port refuse such a unit on every entry path found. Grinders and absorbers such as the Bio Reactor still take it in. Docking that keeps the unit outside is unchanged: repair depot, refinery, weeder, helipad and `UnitReload` pad.

Here "controlled" means `TechnoClass::MindControlledBy != NULL`. The branch does not refuse a unit the psychic dominator took. `code/psydom.cpp:161` frees that unit from its controller, which clears `MindControlledBy`, before it sets `IsPermaControlled`, so from then on the unit belongs to the house. See question 2.

Nothing here was built or run in the game. All behavior below comes from reading the code.

## Commits

| Commit | What it does |
| --- | --- |
| `3a644818` Refuse mind-controlled passengers in every transport | `TechnoClass::Can_Fit_Passenger` returns false for a controlled passenger. This covers the vehicle and aircraft `RADIO_CAN_LOAD` and `RADIO_DOCKING` answers, the cursor (`Transport_Enter_Action`) and both arrival checks. |
| `8def912f` Keep mind-controlled units out of garrisons and tank bunkers | `BuildingClass::Can_Be_Occupied_By` and `Can_Bunker` return false for a controlled soldier or vehicle. This covers the cursor, the order, the mission re-checks, arrival and the team scripts. |
| `faddaa55` Keep mind-controlled soldiers out of hospitals and armories | A `Hospital=yes` or `Armory=yes` structure answers `RADIO_CAN_LOAD` from a controlled soldier with `RADIO_NEGATIVE`. |
| `9df2b548` Stop mind-controlled engineers and spies entering structures | Cursor: the engineer table and the shared `Infiltrate=yes` rule give `ACTION_NO_ENTER`. Arrival: when a controlled soldier on capture, area guard or patrol walks into its target structure, it drops its target and destination, goes idle and steps aside. No trigger springs. |
| `c38647a5` Let a team load without its mind-controlled members | `TeamClass::TMission_LOAD` no longer sends a controlled member to the transport. The transport now refuses that member, so without this change the script step would assign the enter mission again forever. |
| `d2d58c9e` Document that mind-controlled units cannot enter most things | Adds a section to `manual/content/systems/mind-control.md`. Adds a short condition with a link to the transports, garrisons, tank bunkers, capture and repair system pages and to the `Hospital` and `Armory` key pages. Adds the change record `manual/changes/yr-mind-control-entering.md` (fix, 0.2.0). |
| (this log) | |

The hospital and armory change and the engineer and spy change are in separate commits. If the original game allows those entries, either commit can be dropped (question 3).

## Entry paths checked

Line numbers refer to this branch.

| Path | File:line | Result |
| --- | --- | --- |
| Transport check that every transport path below uses | `code/techno.cpp:7713` `Can_Fit_Passenger` | Refuses (new) |
| Vehicle transport `RADIO_CAN_LOAD` / `RADIO_DOCKING` | `code/unit.cpp:1213`, `:1282`, `:1301` | Refuses through `Can_Fit_Passenger` |
| Aircraft transport `RADIO_CAN_LOAD` / `RADIO_DOCKING` | `code/aircraft.cpp:2689`, `:2656` | Refuses through `Can_Fit_Passenger` |
| Transport cursor for infantry and vehicles | `code/foot.cpp:4744` `Transport_Enter_Action` | `ACTION_NO_ENTER` (the transport answers `RADIO_NEGATIVE`) |
| Vehicle arriving in a transport | `code/unit.cpp:2269` | Refuses |
| Infantry arriving in a transport | `code/infantry.cpp:1015` (`RADIO_CAN_LOAD`) | Refuses; the soldier switches to guard and scatters |
| Enter mission toward a transport | `code/foot.cpp:2443` (`RADIO_DOCKING`) | Refuses; the mission ends |
| Team script: load onto transport | `code/team.cpp:2904` `TMission_LOAD` | Skips controlled members (new) |
| Team script: wait until fully loaded | `code/team.cpp:3508` | Unchanged; see "Not covered" |
| Garrison check | `code/building.cpp:8582` `Can_Be_Occupied_By` | Refuses (new) |
| Garrison cursor | `code/infantry.cpp:2959` | No enter cursor; the soldier gets the cursor it would get over any structure it cannot garrison |
| Garrison enter mission re-check | `code/foot.cpp:2410` | Refuses; the soldier idles |
| Garrison arrival | `code/infantry.cpp:799` | Refuses; the soldier steps aside |
| Team script: enter battle bunker | `code/team.cpp:4135` | Member not sent |
| Tank bunker check | `code/building.cpp:8644` `Can_Bunker` | Refuses (new) |
| Tank bunker cursor | `code/unit.cpp:4485` | `ACTION_NO_ENTER` |
| Tank bunker arrival | `code/unit.cpp:2195` | Refuses |
| Enter mission toward a grinder, absorber or bunker | `code/foot.cpp:2383` `Takes_Walk_Ins` | Bunker refuses; grinder and absorber allow |
| Team script: enter tank bunker | `code/team.cpp:4111` | Member not sent |
| Hospital / armory `RADIO_CAN_LOAD` | `code/building.cpp:444` | Refuses (new) |
| Hospital / armory cursor | `code/infantry.cpp:3051`, `:3070` | `ACTION_NO_ENTER` through `RADIO_CAN_LOAD` |
| Engineer cursor: repair, capture, bridge hut | `code/infantry.cpp:2983` | `ACTION_NO_ENTER` (new) |
| Shared `Infiltrate=yes` cursor: spy, or engineer over a non-repairable structure | `code/infantry.cpp:3142` | `ACTION_NO_ENTER` (new) |
| Engineer, spy or other `Infiltrate=yes` soldier arriving at a structure | `code/infantry.cpp:854` | Turned away (new) |
| Grinder: cursor, arrival, `RADIO_CAN_LOAD`, `Takes_Walk_Ins` | `code/infantry.cpp:2963`, `:989`; `code/unit.cpp:2191`; `code/building.cpp:428`, `:8622` | Allowed, unchanged |
| Bio Reactor (`Can_Absorb`): cursor, arrival, team script | `code/building.cpp:8701`; `code/infantry.cpp:2968`, `:994`; `code/unit.cpp:2202`; `code/team.cpp:4123` | Allowed, unchanged |
| Computer choices 2 and 3 | `code/capture.cpp:235` `Send_To_Grinder` | Allowed, unchanged |
| Repair depot, refinery, weeder, helipad and `UnitReload` docking | `code/building.cpp:434-470`; `code/unit.cpp:663`, `:1670`, `:5027`, `:5748`; `code/techno.cpp:10334`; `code/aircraft.cpp:1974`, `:2611`, `:3532` | Docking; unchanged |
| Vehicle thief taking a vehicle | `code/infantry.cpp:820`, `:3035` | Unchanged; see question 4 |
| Engineer walking into a deployed vehicle | `code/infantry.cpp:820` | Unchanged; see question 4 |
| Carryall lifting a vehicle | `code/aircraft.cpp:1476` | Unchanged (Tiberian Sun carryall; nothing enters) |
| Reinforcements created aboard a transport | `code/reinf.cpp:115` | Unchanged (a new reinforcement cannot be controlled) |

## A unit already on its way in

When a unit is taken over, `CaptureManagerClass::Capture_Unit` (`code/capture.cpp`) gives it `MISSION_GUARD`. A harvester that is unloading is the only exception. If the new owner is a computer house, `Decide_Unit_Fate` then picks a new order. A unit heading into a transport or structure therefore stops when it is taken over. The capture also changes its house, so a transport of its old side answers `RADIO_STATIC`. The garrison, bunker, transport and capture paths also check again on arrival, so no separate handling was added.

## Not covered

- A vehicle that is already in a tank bunker can still be mind-controlled. It stays in the bunker under its new owner, and nothing ejects it (question 5).
- A computer house's controlled engineer on hunt still picks capturable structures (`code/foot.cpp:986`, `code/infantry.cpp:2861`). It walks to one, is turned away at the door and may pick the same structure again. A team `TMISSION_SPY` step behaves the same way (`code/team.cpp:1720`). The loop does no harm but wastes the unit's time. Dropping `THREAT_CAPTURE` for a controlled engineer would fix it. That change was left out because it changes target selection.
- `TMission_FULLY_LOADED` (`code/team.cpp:3508`) waits until every transport is full and does not check which members boarded. A team whose only members left to board are controlled can still wait forever. Before this branch, a team with members too large for its transport could already wait forever in the same way.

## Questions for the owner

1. **Grinders.** In the `AICapture` table, choice 2 sends a controlled unit to a `Grinding=yes` structure, and `code/capture.cpp` does the same. That suggests the original game lets a controlled unit into a Grinder. This branch keeps Grinders open, along with `InfantryAbsorb` and `UnitAbsorb` structures. Can a mind-controlled unit enter a Grinder in Yuri's Revenge?
2. **Dominated units.** This branch treats a unit the psychic dominator took as the house's own, so it may enter anything. Is that right?
3. **Hospitals, armories, engineers and spies.** This branch treats all of these as entering a structure. A controlled soldier cannot use a hospital or armory, and a controlled engineer or spy cannot repair, capture or infiltrate. Does the original game refuse these too? Each one is in its own commit (`faddaa55`, `9df2b548`).
4. **Vehicle thieves and deployed vehicles.** A controlled vehicle thief can still take a vehicle. A controlled engineer can still walk into a deployed vehicle, because the capture code handles a deployed vehicle as a vehicle. Should either be refused?
5. **A bunkered vehicle taken over.** Should a vehicle in a tank bunker leave the bunker when it is mind-controlled?

## What ran

- `clang++ -std=c++20 -fsyntax-only -Icode code/techno.cpp` stopped at `code/motlib.h:14` because `windows.h` was not found. No engine file was compiled; I re-read the diff instead.
- `python3 manual/tools/manage.py update` passed.
- `python3 manual/tools/manage.py check` passed every validation. Its only action item was the expected "site dependencies are missing".
- `git checkout -- manual/data/commands.yaml manual/data/scripting.yaml` had no changes to discard.
- Not run: any build, unit tests, the game or the in-game autotests.

## Owner's answers

- Grinders keep taking controlled units, as implemented.
- A controlled engineer may enter a non-allied `Capturable=yes` structure to capture it. "Let mind-controlled engineers capture Capturable structures" restores the capture cursor (`code/infantry.cpp`, the engineer rules and the `Infiltrate=yes` rule) and lets the engineer go in on arrival (`InfantryClass::Per_Cell_Process`). A controlled engineer still may not repair an allied structure or a bridge hut, and a controlled spy or other `Infiltrate=yes` soldier still may not go in. The mind-control and capture pages and the change record say so.
- Later answer: a controlled spy may infiltrate and a controlled engineer may repair. "Let mind-controlled engineers and spies enter structures again" removes every engineer and `Infiltrate=yes` check, so `code/infantry.cpp` matches `yr`, and the capture page is back to its `yr` text. Transports, garrisons, tank bunkers, hospitals and armories still refuse a controlled unit; Grinders and Bio Reactors still take it.
