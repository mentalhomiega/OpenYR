# Review of buildings, economy and superweapons, 2026-10-15

Branch `cloud/review-buildings-2026-10-15`, begun from `origin/modern` (`4d36e67d`). Job 3 of `CLOUD_JOBS.md`. No earlier log covers this job.

## What was read

`git log --oneline 1cb8bf7..origin/modern -- <the listed files>` lists 201 commits, 191 of them without merges. I read the diff of each of the 191, limited to the listed files (`building.cpp/.h`, `builtype.cpp/.h`, `super.cpp`, `psydom.cpp`, `lstorm.cpp`, `slaveman.cpp`, `rules.cpp/.h`, `const.cpp`, `house.cpp`). Where a diff raised a question I read the surrounding code at `origin/modern`, its callers, and the manual page that owns the claim. I did not read the 10 merge commits as such, and I read the manual pages and change records only where a diff raised a question; I did not audit every page those commits touched (job 8 does that).

## Commits on the branch

| Commit | What it does |
| --- | --- |
| `b2079273` | The spy row of `capture.md`, its change record and the `SuperClass::Reset` comment said a suspended superweapon "keeps a full charge". The code sets `Control` to the full recharge time and stops it, which is an empty charge (`SuperClass::Enable` and `Recharge` use the same value for "not charged", and the cameo stage is `(total - Control) / total`). The prose now says the charge is emptied and the countdown stays stopped. Behaviour is unchanged. |
| `8ba75123` | `HouseClass::Can_Replace_Node` tested `node.CellID == CELL_NONE` after `e773e600` removed `node.CellID == Cell(0, 0)` to apply the hold-back to a node at cell 0,0. `CELL_NONE` is `Cell(0,0)` (`code/globals.cpp:88`), so that commit changed nothing. The test is gone; a placed node without a cell is now held back like any other placed node. Change record added. |
| `cc3fa050` | `Can_Replace_Node` rebuilds a structure with `ResourceGatherer=yes` and `ResourceDestination=yes` (the deployed slave miner) by the `AISlaveMinerNumber` count, not by the timer. `ai-base-building.md` did not list the rule, the key page summary named "walls, base defenses and power plants", and the `AIRestrictReplaceTime` comment in `rules.h` repeated that. All three now match the function. Documentation only. |

`manage.py update` then `manage.py check` ran before each commit; `check` ended with only the expected "site dependencies are missing" line. `update` also regenerates 153 lines in `manual/data/ini-keys.yaml` that no commit of mine caused (the Beta 23 recoil keys were never regenerated on `modern`). `check` fails with "invalid see_also target" for them until `update` runs, so `modern` needs one `update` commit. I discarded that churn from my commits.

## Findings that still need a decision

Each needs gamemd evidence or a play test, so none is fixed here.

1. `code/house.cpp:10180`, `HouseClass::Pick_Drop_Target`. `2b88bf11` moved `cell = cell + Cell(2, 2)` ahead of `if (cell == CELL_NONE)`. `Map.Nearby_Location` reports failure as `Cell(0,0)`, so the test can no longer be true, and a failed search now aims the paradrop, spy plane or psychic reveal at cell (2,2), a map corner. Failing case: a computer house whose enemy starting cell has no free 5x5 area. Fix: test before adding, or delete the dead test if gamemd does add first.
2. `code/building.cpp:8650`, `BuildingClass::How_Many_Survivors`. The comment says "Civilians hold the third place in the stock rules, so they use the third divisor". `capture.md` and `thirdsurvivordivisor--global-rules.md` say the stock `[Sides]` order is GDI, Nod, ThirdSide, Civilian, Mutant, so a civilian house gets none. One of them is wrong. Fix: check the stock `[Sides]` order, then correct the comment or the pages.
3. `code/house.cpp:3179`, `HouseClass::AI_Super_Weapon_Defense`. `2b88bf11` measures from `Starting_Cell_Or_Default()`, which is the origin when the house has no starting cell. `manual/content/systems/superweapons.md:415` still says the distance is measured from "the center of the computer house's base" and that a house "with no base does not react". Fix: after a play test, describe the starting cell and the origin default in that page and in `aisuperdefensedistance--global-rules.md`.
4. `code/building.cpp:11175`, `BuildingClass::Is_Powered_On`. `31531291` calls `Overpowerer_Count()`, a scan of every object in `Technos`, whenever the structure is switched off or its house is short of power. `Is_Powered_On` is called from `Can_Fire`, `AI`, gap and cloak code, so a low-power match with many structures scans `Technos` many times a frame. Fix: count only when `Class->IsOverpowerable`, or keep the count in a member that `BuildingClass::AI` updates (it already computes it at line 1771 for `IsOverpowered`).
5. `code/building.cpp:4421`, `BuildingClass::What_Action`. A gap generator with `SuperGapRadiusInCells=0` still gets the deploy cursor, and a click sends a `DEPLOY` event that does nothing (`Do_MISSION_UNLOAD`, line 7129, ignores it). `supergapradiusincells.md` says only that a value of `0` keeps the click from changing anything, so the page is accurate but the cursor is misleading. Fix: add `Class->SuperGapRadiusInCells != 0` to the test if gamemd does the same.
6. `code/building.cpp:7129`, `Do_MISSION_UNLOAD`. The check that the house has the power for the charge (`Power - Drain > ExtraPowerDrain`) is in the cursor only. A `DEPLOY` event that arrives after power fell, or from a replay or a modified client, charges the generator anyway. The house then falls short for the frames until `BuildingClass::AI` discharges it. Behaviour stays deterministic. Fix: repeat the test in `Do_MISSION_UNLOAD`.
7. `code/house.cpp:7371`, `Tracking_Active_Remove`/`Tracking_Active_Add`. A `DontScore=yes` unit is counted into `AUQuantity` on entry and never taken out, so the count only grows (a deliberate port, per the comment). Anything that reads `AUQuantity` for such a type (build limits, `Count_Owned`) sees a count that never falls. Fix: none unless a rule type that is `DontScore=yes` also has a `BuildLimit`; check the stock rules.
8. `code/building.cpp:9346`, `Can_Be_Occupied_By`. The mind-control check (`MindControlledBy != NULL`) sits after the Occupier branch, so a mind-controlled `Assaulter=yes` soldier that is not an `Occupier` can still enter a garrison to kill its occupants. The doc comment says a mind-controlled soldier may not garrison. Fix: move the check above the Assaulter branch if gamemd refuses it too.
9. `manual/changes/survivor-count-from-refund.md`. The example says a first-side structure with a 1,000-credit refund "releases two survivors" with "the shipped values". The engine default is 200 (five survivors); the example holds only if the stock rules set 500, as `alliedsurvivordivisor--global-rules.md` implies. Fix: name the value in the example.
10. Save and sync, for job 4: `BaseNodeClass::Attempts` (`code/base.h:80`) is saved but not in `BaseClass::Compute_CRC` (`code/base.cpp:576` adds `Placed` only), and `Attempts` decides when `Exit_Object` deletes a node. I did not check the other fields these commits add (`CountResourceGatherers`, `CountResourceDestinations`, `BaseSpawnCell`, `DefenseFactory`, `ShipFactory`, `OwnCountry`, `PreferredDefensiveCell*`) against the CRC or the load order; that is job 4's work. `HasEngineer`, `IsC4Applied`, `GapRadius`, `IsGapSuperCharged`, `HasExtraPowerDrain`, `SecretProduct` and `Base_Edge` are in the CRC.

## Checked and left alone

- Docked-object loops in `Change_Owner`, `Do_MISSION_REPAIR`, `Do_MISSION_DECONSTRUCTION` and `Take_Damage` (`RESULT_DESTROYED`) copy the link list or re-read `Link(slot)` each pass, so a docker removed by an earlier step is skipped.
- `Kill_Occupants`, `Eject_Occupants(true)` and `Destroy_Selected` cannot loop forever: the first two copy the list and clear it, the third skips an object that survives its hit.
- `Map[cell]` returns a scratch cell off the map, so `Exit_Naval_Object` and `Can_Replace_Node` cannot index outside the array. `CounterClass::Value` range-checks, so `Owns_Prerequisite_Override` is safe with group codes.
- `Free_All` and `TechnoClass::Record_The_Kill` accept a null killer.
- `Power_Output`'s one-lower rule assumes gamemd chops the ratio toward zero; I could not check that without the executable.
- `AISuperDefenseProbability` passes with chance `P + 1` percent for `P` below 100. `aisuperdefenseprobability--global-rules.md` documents exactly that.
- `Grand_Opening(capture)` does nothing for an already opened structure that a trigger or mind control hands over, so no free unit is paid twice.

## What ran and what did not

- Ran: `git fetch`, `git log -p`, reading the code and manual pages above, `python3 manual/tools/manage.py update` and `check` before each commit (results above), and `git checkout` of the regenerated data files.
- Did not run: a build, any unit test, the game. The engine does not build on Linux and nothing I changed has test coverage; the one code change (`8ba75123`) is a one-condition deletion. The shared scratchpad directory is used by other sessions and one of them overwrote my first diff file, so I regenerated it under `scratchpad/rb-buildings/`.

## Ideas for the next session

- Run `manage.py update` on `modern` and commit the regenerated `ini-keys.yaml`.
- Job 8 should check the AI base-building pages (`ai-base-building.md`, `superweapons.md`) against `house.cpp` after the starting-cell commits, since those commits (`035a86a1` to `efe4ff4d`) changed distances and zones without a manual change.

## Questions for MentalHomiega

- Does gamemd add (2,2) before testing for a failed search in the paradrop target routine (finding 1)?
- What is the stock `[Sides]` order, and does a civilian house release survivors (finding 2)?
- Does a suspended superweapon that a spy enters really come back with an empty charge, or is gamemd's timer shown as full on the cameo (`b2079273`)? I changed the prose to match the code, not gamemd.
