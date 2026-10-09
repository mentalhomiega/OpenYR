# Cloud jobs

Ready-made prompts for cloud sessions started by hand on `mentalhomiega/OpenYR`. The local session keeps this list stocked: it removes a job once its branch is pushed and adds new ones as work comes up.

## How to start one

1. Start a cloud session on `mentalhomiega/OpenYR` (Cloud, then the Continuation environment).
2. Choose **Sonnet 5.5 at high effort**. Opus and max effort cost several times more and gain little on these jobs.
3. Paste the shared opening below, then one job from the list under it. Take the first open job; several sessions can run at once if each takes a different job.

Each job is sized for one session and ends at a stated point. Sessions started this way use the cloud session credits, which run until 5 November.

## Shared opening

```text
You are working on the GitHub repository mentalhomiega/OpenYR, a fork of OpenTS that rebuilds the Red Alert 2: Yuri's Revenge engine. Run `git fetch origin` and `git checkout modern`, then read CLOUD_BRIEFING.md at the repository root and follow its hard rules, its session-log section and its report section. You cannot run the game and there are no game files; never add game assets, binaries or decompiled code. Never write the owner's real name anywhere; the owner is MentalHomiega, and commits use `MentalHomiega <182634060+mentalhomiega@users.noreply.github.com>` (set it with git config before committing). Commit in small commits with imperative subjects of at most 72 characters and no AI-attribution or Co-authored-by lines. Begin your branch from `origin/modern` unless the job says otherwise, and push only to the new branch named in the job (add today's UTC date where it says YYYY-MM-DD); never push to `modern`, `yr` or `main`, never force-push, delete branches or open pull requests, issues or comments anywhere. Before finishing, add your log as `cloud-log/YYYY-MM-DD-<job>.md` on your branch, then end with the briefing's report.

Your job:
```

## Open jobs

Build 08 for play-testers was made from `1cb8bf7`. Since then over 100 commits on `modern` have ported gamemd behaviour found by local agents: AI teams and triggers, unit abilities, veterancy, crates, superweapons, buildings and the ore economy. They were tested in the real game but not reviewed line by line, so the first four jobs review them.

### 1. Review: AI, teams and triggers

```text
Review the `modern` commits since `1cb8bf7` that touch the computer player, teams, scripts and triggers: run `git log --oneline 1cb8bf7..origin/modern -- code/team.cpp code/team.h code/teamtype.cpp code/teamtype.h code/aitrig.cpp code/aitrig.h code/house.cpp code/house.h code/reinf.cpp code/scenario.cpp code/capture.cpp code/_script.cpp code/script.cpp code/tevent.cpp code/taction.cpp code/tag.cpp`. For each commit, look for logic errors, missed cases (a dead or limbo object, a team with no members, a house that has been defeated), null pointers, iterator invalidation while units are removed, and comments or manual pages the code does not support. Fix only what the code itself proves wrong, one fix per commit with its manual update; list everything else with file, line, the failing case and a suggested fix. Stop when every commit in the list has been read. Branch: cloud/review-ai-YYYY-MM-DD.
```

### 2. Review: units, infantry, aircraft and combat

```text
Review the `modern` commits since `1cb8bf7` that touch units and combat: run `git log --oneline 1cb8bf7..origin/modern -- code/techno.cpp code/techno.h code/unit.cpp code/unit.h code/infantry.cpp code/infantry.h code/foot.cpp code/jumpjet.cpp code/aircraft.cpp code/veteran.cpp code/weapon.cpp code/weapon.h code/parasite.cpp code/spawnman.cpp code/temporal.cpp code/cell.cpp code/bullet.cpp code/combat.cpp code/infatype.cpp code/techtype.cpp`. For each commit, look for logic errors, rounding and integer-overflow mistakes in damage, experience and speed maths, objects used after they are destroyed or put in limbo, null pointers, and comments or manual pages the code does not support. Fix only what the code itself proves wrong, one fix per commit with its manual update; list everything else with file, line, the failing case and a suggested fix. Stop when every commit in the list has been read. Branch: cloud/review-units-YYYY-MM-DD.
```

### 3. Review: buildings, economy and superweapons

```text
Review the `modern` commits since `1cb8bf7` that touch buildings, money and superweapons: run `git log --oneline 1cb8bf7..origin/modern -- code/building.cpp code/building.h code/builtype.cpp code/builtype.h code/super.cpp code/psydom.cpp code/lstorm.cpp code/slaveman.cpp code/rules.cpp code/rules.h code/const.cpp code/house.cpp`. For each commit, look for logic errors, money or power that can go negative or overflow, buildings that are sold, captured or destroyed partway through an action, null pointers, and comments or manual pages the code does not support. Fix only what the code itself proves wrong, one fix per commit with its manual update; list everything else with file, line, the failing case and a suggested fix. Stop when every commit in the list has been read. Branch: cloud/review-buildings-YYYY-MM-DD.
```

### 4. Save-game and sync audit

```text
The save revision in `code/savever.h` went from 19 to 31 since `1cb8bf7` as new fields were added to game classes. Run `git log -p 1cb8bf7..origin/modern -- 'code/*.h'` and list every data member added to a class that is saved. For each one, check that it is initialised in every constructor (including the one used when a save is loaded), written and read by the class's save and load code in the same order, and included in the class's CRC or sync calculation if it affects the simulation. Fix only what is missing, one class per commit, bumping `REVISION` once for the whole branch if the save layout changes. List the fields you checked in your log, each with its verdict. Stop when every new field has a verdict. Branch: cloud/save-audit-YYYY-MM-DD.
```

### 5. Research: specs for the next priorities

```text
Follow docs/research/RESEARCH_BRIEFING.md, output 4 (specs). Read docs/research/priorities.md and docs/research/specs/README.md, then write specs for the three highest-ranked feature groups that have no spec yet. The ranking currently puts first the Ares super weapon keys (`SW.*`, `EVA.*`, `Message.*`, `Money.*` and the per-type keys), then the Ares building keys mods use most (`Foundation.*` and `FoundationOutline.*` custom shapes, `UC.*` units passing between buildings), then Phobos shields (`ShieldType`). For each, give where it hooks into `code/` on `modern`, the saved state it needs, and a test plan we can run in the real game. Stop after three specs. Branch: cloud/research-YYYY-MM-DD, begun from the newest `origin/cloud/research-*` branch merged with `origin/modern`.
```

### 6. Research: finish the tables and the scanner

```text
Follow docs/research/RESEARCH_BRIEFING.md. Continue from the newest `origin/cloud/research-*` branch and its log in `docs/research/log/`. Do the log's "Next session" items that need no game files: the two `mod_scan.py` changes the briefing describes (reading rules from unencrypted MIX archives in memory, and the "stock Yuri's Revenge, not read by this engine" origin), each with tests; table the Phobos AI scripting and miscellaneous pages; and tick the Ares bug fix rows that `code/` on `modern` already handles, citing the function. Stop when those are done. Branch: cloud/research-YYYY-MM-DD, begun from the newest `origin/cloud/research-*` branch merged with `origin/modern`.
```

### 7. Unit tests for the new rules maths

```text
Add unit tests that need no game files for self-contained calculations added to `modern` since `1cb8bf7`: veterancy multipliers and the experience a kill is worth by the victim's rank, the cell spread table used for area damage, prone infantry damage rounding, ore purifier bonus rounding, `HarvestersPerRefinery`, and production speed under low power. Find each with `git log --oneline 1cb8bf7..origin/modern` and the code it changed. Use the existing test setup in `tests/`. If a calculation needs too much engine state, test the smallest piece you can lift out without changing behaviour, or say so in your log. Stop when each calculation has tests or a stated reason. Branch: cloud/tests-YYYY-MM-DD.
```

### 8. Manual audit of the new change records

```text
For every file added to `manual/changes/` since `1cb8bf7` (`git diff --name-only --diff-filter=A 1cb8bf7 origin/modern -- manual/changes`), and the key and system pages those commits changed, check each claim against the current source on `modern`. Narrow or correct any claim the code does not support, following `manual/AGENTS.md` and the prose rules in `AGENTS.md`. Keep each page's fix in its own commit. Stop when every new record has been checked. Branch: cloud/manual-audit-YYYY-MM-DD.
```

## Waiting

These need earlier cloud branches merged into `modern` first. The local session moves them up when that is done.

- **Static analysis follow-up** (from `cloud/static-2026-10-06`).
- **INI checker, next stage** (from `cloud/inicheck-2026-10-06`).

## Done

Earlier jobs whose branches are pushed and wait for the local session to test and merge: research (4 and 6 October), INI checker, static analysis, unit tests, docks and radio, mind-controlled units entering buildings, the manual CI check, and the nightly combined branches up to 7 October.
