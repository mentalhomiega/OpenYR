# Review of the units and combat commits (1cb8bf7..4d36e67d)

Branch: `cloud/review-units-2026-10-15`, started from `origin/modern` (4d36e67d). The clone was shallow and did not know `1cb8bf7`; after `git fetch origin` and a failed `git fetch --unshallow` the commit resolved, and the job's `git log` command then listed 265 commits.

## What I read

All non-merge commits in that list, as the diff of the job's file list, oldest first (about 4200 added and 1600 removed lines in `techno`, `unit`, `infantry`, `foot`, `jumpjet`, `aircraft`, `veteran`, `weapon`, `parasite`, `spawnman`, `temporal`, `cell`, `bullet`, `combat`, `infatype` and `techtype`). The merge commits add nothing of their own. I followed the damage, experience, speed and build-time arithmetic through the current source and compared the rank, damage, speed and prone-damage pages in the manual with it.

## Commits

| Commit | What it does |
| --- | --- |
| Document that prone damage is rounded down | 493b1eef put `InfantryClass::Take_Damage` back to truncating the prone share, but `ProneDamage`, the warhead damage order and the change record still said "rounded to the nearest point". Both pages now say rounded down. `changes/prone-damage-rounds-to-nearest.md` cannot be deleted (`manage.py check` refuses to drop a change ID), so it is retitled "Keep prone damage rounded down" and says what the code does. |
| Correct the veteran armor formula in the manual | The warheads page divided by "one plus `VeteranArmor`"; ac4ac18a made the code divide by `VeteranArmor`. `VeteranArmor` also said "the house's armor multiplier", which dbed91fd replaced with the country's. |
| Correct the veteran speed formula in the manual | `movement-and-terrain` said "`VeteranSpeed` plus one", and `veterancy` and `VeteranSpeed` still named the house ground-speed bias, which 29526d7b removed from `FootClass::Current_Speed`. |
| Correct the veteran firepower order in the manual | `VeteranCombat` said it "applies last". `TechnoClass::Fire_Damage` applies the bunker, garrison and open-topped multipliers after it. |

All four are page-only changes to text that an earlier commit left stale, so none needs a new change record. `manage.py update` then `check` passed; `update` also rewrote `ini-keys.yaml` (the recoil keys `review-beta23` already adds), which I discarded.

## Findings that still need a fix

None of these is proven by the code alone to fail in play, so I left the source untouched.

| Where | Case | Suggested fix |
| --- | --- | --- |
| `code/techno.cpp` `Start_Drain` (11685) and `Stop_Drain` (11709) | 77d3a8e0 removed the `target->DrainingMe != NULL` guard so a second drainer takes over, but `Stop_Drain` still clears `DrainTarget->DrainingMe` unconditionally. After a takeover, the first drainer's later `Stop_Drain` (it moves off, dies or is deleted) clears the second drainer's link while that drainer still sits on the structure: the power or money drain ends, and the second drainer's `Stop_Drain` finds nothing to clear. The old drainer also keeps `DrainTarget` and its animation until then. `Can_Fire` still refuses a drained structure and the `DrainWeapon` page says so, so I could not construct a route to the takeover. | In `Stop_Drain`, clear `DrainingMe` and set `RecalcPower` only when `DrainTarget->DrainingMe == this`. In `Start_Drain`, stop the previous drainer first. |
| `code/spawnman.cpp` `Unlink_Pointer` (161) with `TechnoClass::Delete_Me` (techno.cpp 8118) | d6f56bb9 keeps the node of a living, non-kamikaze, non-missile aircraft so a spawn that docks (Limbo) stays linked. `Delete_Me` also reaches `Unlink_Pointer` through `Detach_This_From_All` while the aircraft is still `IsActive` with strength above zero, so the node keeps a pointer to an object that is queued for deletion. `TemporalClass::Update` (temporal.cpp 196) does exactly that to any target: `Record_The_Kill` then `Delete_Me`, no strength change. `SpawnManagerClass::AI` then reads `node.Unit` (status checks, `Send_Home`) after it is freed. | In `TechnoClass::Delete_Me`, when `SpawnOwner` has a `SpawnManager`, release the node for this unit before `BASECLASS::Delete_Me()`; or have `Unlink_Pointer` test a flag that `Delete_Me` sets first. |
| `code/jumpjet.cpp` `Process_Cruise` (559) | `Facing.Difference().As_Dir256()` is `Desired - Current` modulo a full turn. A destination 10 units left of the heading gives 246, not 10, so `turn_rate < turn` and `turn_rate * 5 < turn` hold and the jet slows to `speed / 5` or `speed / 10` for every left turn, however small. A right turn of 10 does not. Cannot tell whether gamemd wraps the same way. | Use the shorter way round, `std::min(turn, 256 - turn)`, if gamemd does not wrap. Check on Windows with a jumpjet sent to the left and to the right of its heading. |
| `code/jumpjet.cpp` `Process_Cruise` (577) | In the `distance < speed * 50 / turn_rate` band the code sets `FlightLevel = distance`. A type with a large `JumpjetSpeed` and a small `JumpjetTurnRate` has a band wider than `JumpjetHeight` (speed 40, rate 1 gives 2000 leptons), so the jet climbs above its cruise height when it enters the band, then comes down. | Clamp to `JumpjetHeight` if gamemd does. |
| `code/unit.cpp` `Simple_Deploy_AI` (7352), `code/jumpjet.cpp` (371, 658) | `IsLandingToDeploy` is set when a flying DeployToLand unit starts to land and cleared only when the jet reaches `GROUNDED`. A Siege Chopper told to move or stop during the descent keeps the flag, so `Hovers_At_Stop` is false and the next stop makes it descend and land instead of hover. It lands once, which clears the flag. | Clear the flag when the unit's mission or destination is changed away from the landing, or test `Mission == MISSION_UNLOAD` as well. |
| `code/techno.cpp` `Estimated_Damage` (4890) | `Modify_Damage(estimate, weapon->WarheadPtr, ...)` dereferences the warhead. `Target_Something_Nearby` checks `weapon->Bullet` but not `WarheadPtr`, so a weapon with no `Warhead=` crashes the scan. The divisor uses the shooter's `TClass` and `ArmorBias` with the target's house; the comment says that is gamemd's order, which I cannot check. | Return 0 when `WarheadPtr` is NULL. Confirm the divisor on Windows. |
| `code/techno.cpp` Disk laser branch (5035) | bcf60cba changed `Arm = IsBerzerk ? Rearm_Delay / 2 : Rearm_Delay` to `Arm = Rearm_Delay(which)` for the Floating Disc. The ordinary path (`Fire_At`, `IsBerzerk` halving) still halves it. A berzerk disc therefore rearms at full ROF. The commit does not mention berzerk. | Restore the halving unless gamemd's disc path has none. |
| `code/bullet.cpp` direct-rocker branch (1595) | `toward.Normalize()` on a zero vector gives NaN, and the NaN goes into `Coord(...)`. It needs the firer and the target at the same position (a spawner or limbo-launched firer over the unit it hits). | Skip the rock, or use zero, when the length is zero. |
| `code/foot.cpp` `Approach_Target` (1267) | 3c936de5 and 1ef74d5c drop the chosen spot with `NavCom = NULL`, which leaves the old `Path` and the locomotor moving to the old spot until the next `Assign_Destination`. Other code calls `Assign_Destination(NULL)` to cancel. | Call `Assign_Destination(NULL)` there. |
| `code/techno.cpp` `Take_Damage` (6392) | `VeteranArmor=0` gives `(int)(damage / 0.0)`, which is undefined. The `VeteranArmor` and `veterancy` pages already say to keep it above zero. | Treat a value at or below zero as 1 when reading `rules.ini`. |

## Checked and found sound

- Experience and kills: `VeterancyClass::Made_A_Kill` (float32 total, NaN and infinity reach the cap), `Record_The_Kill` (allies worth nothing, double for a veteran victim, triple for an elite, `DontScore` gate, credit to transport, launcher or occupant), `Record_The_Kill_By_House`.
- Rank multipliers now match their pages for speed, sight, rate of fire, firepower and armor, and the rules defaults are 1.
- Damage order in `Take_Damage`, `Modify_Damage` (double precision, float32 edge), `Explosion_Damage` (house passed to every victim) and the prone share.
- `Cell_Spread_Offset` holds 369 entries, equal to `Cell_Spread_Count(11)`. The doubled `(-3, 11)` entry is the one the comment names. `Shrapnel` stays inside that table for any weapon range.
- Build time: `Time_To_Build` truncates each step, floors a zero speed at float32 0.01, and `Country_Build_Time_Multiplier` is applied once.
- Temporal gunner hand-over: `Limbo()` lets go of the passenger's target before `Receive_Gunner`, so `Target->WarpedBy` never names a gunner whose warp moved.
- Passengers of a destroyed vehicle, crew and truck crate (`Resolve_Passengers_On_Death`, `Resolve_Crew_And_Crate_On_Death`) run once per wreck, and `Kill_Cargo` now queues deletion instead of `delete`.
- Save format: every field added in the range is in `Serialize`, and `REVISION` is 69 after the "Bump the save revision" commits. `MirageCreationFrame` is not saved by design (drawing only).

## Ran and did not run

- Ran: `git fetch origin`, `manage.py update` and `manage.py check` after each manual change (all verified; "site dependencies are missing" is the expected message).
- Did not run: any build, the unit tests, the game. Every finding above is a reading of the source.

## Questions for MentalHomiega

1. Should `changes/prone-damage-rounds-to-nearest.md` stay as a "fix" with an unchanged result, or should the lifecycle tooling allow a record to be withdrawn before the release?
2. Is a second drainer meant to be able to take a structure over (77d3a8e0), given the `DrainWeapon` page says it cannot?
