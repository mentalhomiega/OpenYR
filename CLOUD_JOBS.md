# Cloud jobs

Ready-made prompts for cloud sessions started by hand on `mentalhomiega/OpenYR`. The local session keeps this list stocked: it removes a job once its branch is pushed and adds new ones as work comes up.

## How to start one

1. Start a cloud session on `mentalhomiega/OpenYR` (Cloud, then the Continuation environment).
2. Choose **Sonnet 5.5 at high effort**. Opus and max effort cost several times more and gain little on these jobs.
3. Paste the shared opening below, then one job from the list under it. Take the first open job; several sessions can run at once if each takes a different job.

Each job is sized for one session and ends at a stated point. Sessions started this way use the cloud session credits, which run until 5 November.

## Shared opening

```text
You are working on the GitHub repository mentalhomiega/OpenYR, a fork of OpenTS that rebuilds the Red Alert 2: Yuri's Revenge engine. Run `git fetch origin` and `git checkout modern`, then read CLOUD_BRIEFING.md at the repository root and follow its hard rules, its session-log section and its report section. You cannot run the game and there are no game files; never add game assets, binaries or decompiled code. Never write the owner's real name anywhere; the owner is MentalHomiega, and commits use `MentalHomiega <182634060+mentalhomiega@users.noreply.github.com>` (set it with git config before committing). Commit in small commits with imperative subjects of at most 72 characters and no AI-attribution or Co-authored-by lines. Begin your branch from `origin/modern` unless the job says otherwise, and push only to the new branch named in the job (add today's UTC date where it says YYYY-MM-DD), with `git push origin HEAD:refs/heads/<that branch>`; never push to `modern`, `yr` or `main`, never force-push, delete branches or open pull requests, issues or comments anywhere. Before finishing, add your log as `cloud-log/YYYY-MM-DD-<job>.md` on your branch, then end with the briefing's report.

Your job:
```

## Open jobs

These jobs build on the 2026-10-15 cloud branches, which are now merged into `modern`. Take them in order.

### A. Fix the findings the reviews listed

```text
Read the "Open" or "Not fixed" lists in `cloud-log/2026-10-15-review-ai.md`, `cloud-log/2026-10-15-review-units.md` and `cloud-log/2026-10-15-review-buildings.md` on `origin/modern`. They include reinforcement edge cases, the dead test in `Pick_Drop_Target`, the `Stop_Drain` takeover case, a dangling spawn node pointer and the jumpjet turn wrap. For each, re-read the code on `modern` (it may have changed since), and fix it when the code itself proves it wrong, one fix per commit with a unit test where one fits and its manual update. Where the right behaviour depends on what gamemd does, list it for us with file, line and the case instead of guessing. Stop when every listed finding has a verdict. Branch: `cloud/review-fixes-YYYY-MM-DD`.
```

### B. Finish the file-reader hardening

```text
Read `cloud-log/2026-10-15-fuzz.md` and `cloud-log/2026-10-15-custom-maps.md` on `origin/modern`. Do the follow-ups they left: structure validation for SHP and VXL frames (offsets and sizes inside the file), the `strtrim` trailing-space case, and every caller of `LCW_Uncomp` that does not check its result or pass an output limit. Each new check must still accept every well-formed file, including unusual ones gamemd loads; when unsure, log and accept rather than reject. Add a regression seed for each fix to the fuzz harness, one commit per reader. Also make the standalone Linux fuzz build compile again with the engine sources the harnesses now need. Branch: `cloud/fuzz-followup-YYYY-MM-DD`.
```

### C. Security follow-ups: setup and launcher

```text
Read `cloud-log/2026-10-15-security-setup.md` on `origin/modern`. Fix the residual hardening findings it lists that can be fixed and tested without Windows, one commit per finding with a test in `tests/setup` or `tests/launcher`. For the rest, and for code signing, write a short plan in your log: what to change, how to test it on Windows, and the risk if we leave it. Branch: `cloud/security-followup-YYYY-MM-DD`.
```

### D. Review: the Beta 24 changes

```text
Review the commits in `d375ab28..origin/modern` that touch `code/`, skipping merges and the 2026-10-15 cloud work you can recognise by its `cloud-log` entries. The main areas are the menu battle behind the menus (`code/shellmap.cpp`, its `BattleScope` state swap, sound gating in `code/voc.cpp`), the Settings screen and tabs (`code/ui/uisettings*.cpp`, `ui/themes/*/`), the mission browser (`code/ui/screens/missionbrowser/`), the hang watchdog (`code/hangwatch.cpp`), the score screens, aircraft waypoints and picking, and the gameplay fixes for slave miners, 1% Verses, voices, sensors, hover health bars and planned waypoint legs. Look for logic errors, objects used after they are freed (the menu battle swaps global state, so check what it misses), thread-safety in the watchdog, and comments or manual pages the code does not support. Fix only what the code proves wrong, one fix per commit; list the rest with file, line and case. Branch: `cloud/review-beta24-YYYY-MM-DD`.
```

### E. Save-game audit since revision 69

```text
`code/savever.h` went from 69 to 73 since the last save audit (`cloud-log/2026-10-15-save-audit.md`). Repeat that audit for data members added to saved classes since then: `git log -p 4d36e67d..origin/modern -- 'code/*.h'`. For each field, check it is initialised in every constructor, saved and loaded in order, and in the CRC if it affects the simulation. The audit also asked whether the plan queue and beacons should be saved: say what gamemd does from the decompiled class layouts in public sources (YRpp headers are fine to read; copy nothing), and implement it if it is a clear gap, bumping `REVISION` once. Branch: `cloud/save-audit-2-YYYY-MM-DD`.
```

### F. Research: check the tables

```text
Follow docs/research/RESEARCH_BRIEFING.md. Begin from `origin/modern` and read the newest log in `docs/research/log/`. Do its "Next session" items that need no game files: spot-check the effect and size of the rows ranked first to seventh in `docs/research/priorities.md` against their sources, merge the two `AnimRemapDefaultColorScheme` rows, and check the 14 ticked rows against the manual and `code/`. Leave the mod scan and the `Foundation.*` and `UC.*` questions for us. Stop when those are done. Branch: cloud/research-tables-YYYY-MM-DD.
```

### G. Unit tests for the newest rules maths

```text
Add unit tests that need no game files for calculations changed on `modern` since `d375ab28`: the passive-acquire Verses threshold (`WarheadTypeClass::Can_Passive_Acquire`: 1% refused, 2% accepted), the mobile sensor circle (`FootClass::Update_Sensors`: x and y over [-r, r-1] with x*x+y*y < r*r), the teleport hold delay (`code/teleport.cpp` `Warp_Delay`), and the menu battle's map-size fit (`code/shellmap.cpp`, the `LocalSize` to pixel conversion). Lift the smallest piece you can without changing behaviour, or say in your log why a calculation can't be tested. Branch: `cloud/tests-2-YYYY-MM-DD`.
```

## Answers we still owe

Questions from earlier logs that wait on MentalHomiega: how Mental Omega's `Foundation.*` keys draw, which Ares version Mental Omega runs, whether the `[Phobos]` RA2MD.INI options get their own page, and whether research sessions should start porting Ares or Phobos behaviour. From the 2026-10-15 logs: whether Civilian is third or fourth in `[Sides]`, whose allies a spy's disguise uses, whether gamemd reads the recoil keys and in what units, which trig library trajectories should use, and whether a tick in the research tables means "has an effect".

## Done

Merged into `modern`: all 14 branches from 2026-10-15 (Beta 23 review, security, fuzzing, custom maps, key index, AI, units and buildings reviews, save audit, research specs, unit tests, manual audit, INI checker, static analysis). Merged into `modern` and `yr` earlier: research (4, 6 and 9 October, including the Phobos specs for items 3 and 5 to 8), INI checker, static analysis, unit tests, docks and radio, mind-controlled units entering buildings, the manual CI check, and the nightly combined branches up to 7 October.
