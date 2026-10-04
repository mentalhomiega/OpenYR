# Cloud jobs

Ready-made prompts for cloud sessions the owner starts by hand. A cloud session started from claude.ai/code (or the "Start a cloud session" button) on `mentalhomiega/OpenYR` uses the cloud session credits; routines do not.

To start one, open a new cloud session on the repository, paste the shared opening below, then one job underneath it. Several jobs can run at once in separate sessions; each pushes its own branch, and the morning test on the owner's PC picks them up.

## Shared opening

```text
You are working on the GitHub repository mentalhomiega/OpenYR, a fork of OpenTS that rebuilds the Red Alert 2: Yuri's Revenge engine. Run `git fetch origin` and `git checkout yr`, then read CLOUD_BRIEFING.md at the repository root and follow its hard rules, its night-log section and its report section. You cannot run the game and there are no game files; never add game assets, binaries or decompiled code. Never write the owner's real name anywhere; the owner is MentalHomiega, and commits use `MentalHomiega <182634060+mentalhomiega@users.noreply.github.com>` (set it with git config before committing). Commit in small commits with imperative subjects of at most 72 characters and no AI-attribution or Co-authored-by lines. Push only to the new branch named in the job below (add today's UTC date where it says YYYY-MM-DD), never to `yr` or `main`; never force-push, delete branches or open pull requests, issues or comments anywhere. Before finishing, add your log as `cloud-log/YYYY-MM-DD-<job>.md` on your branch, then end with the briefing's report.

Your job:
```

## Jobs

### Research: Ares and Phobos tables

```text
Follow docs/research/RESEARCH_BRIEFING.md. Continue from the newest `origin/cloud/research-*` branch and its log. Next: the Phobos tags for Technos and buildings, then interface and animation tags; then merge the Ares table's single-page groups into the systems used in phobos-tags.md. Branch: cloud/research-YYYY-MM-DD.
```

### INI checker, next stage

```text
Continue the INI checker from `origin/cloud/inicheck` (read docs/INICHECK.md and the 2026-10-04 night log on `origin/cloud/nightly-2026-10-04`). Add keys the engine builds from a pattern, such as `DockingOffset%d` and `Weapon%d`, to the catalog export; check weapon, projectile and warhead sections by following `Primary=`, `Secondary=`, `Projectile=` and `Warhead=`; keep it report-only, with tests for each addition. Branch: cloud/inicheck-YYYY-MM-DD, begun from `origin/cloud/inicheck` merged with `yr`.
```

### Static analysis

```text
Do task 5 of CLOUD_BRIEFING.md. Install cppcheck (and clang-tidy if possible) in the container, run it over `code/`, fix only what the code itself proves wrong, one fix per commit, and list the rest with file and line in your log. Branch: cloud/static-YYYY-MM-DD.
```

### Unit tests

```text
Add unit tests that need no game files for the radio slot functions (`code/radio.*`: Set_Link_Count, Find_Link_Index, Has_Free_Link, the HELLO and OVER_OUT slot rules) and for weapon choice (`TechnoClass::What_Weapon_Should_I_Use`, `Naval_Weapon`), using the existing test setup. If a function cannot be tested without large engine state, say so in the log instead of forcing it. Branch: cloud/tests-YYYY-MM-DD.
```

### Dock and radio follow-ups

```text
Fix the open dock and radio findings listed in the 2026-10-04 night log on `origin/cloud/nightly-2026-10-04` (findings 1 and 4 to 11: the carrier spawn leak in `code/spawnman.cpp`, untargeted radio messages that reach slot 0 at a multi-dock building, and the manual wording). The owner's answers in CLOUD_BRIEFING.md apply: a `UnitReload` pad rearms and repairs. Each fix gets its own commit and its manual update. Branch: cloud/docks-YYYY-MM-DD.
```

### Mind-controlled units entering buildings

```text
The owner confirmed that in Yuri's Revenge a mind-controlled unit cannot enter a transport or a structure, except a Bio Reactor. Check every path where a unit can enter something (the cursor and action checks, the order handling, the AI and team scripts) and make the port refuse a controlled unit everywhere except a Bio Reactor. Document it on the mind-control system page with a change record. Branch: cloud/mind-control-enter-YYYY-MM-DD.
```

### Green manual checks on `yr`

```text
The Manual workflow fails on `yr` because upstream tests pin things this fork changed on purpose (see finding 3 in the 2026-10-04 night log on `origin/cloud/nightly-2026-10-04`). For each failing test, update the test to the fork's intended behaviour and say why in the log; restore the pinned behaviour only where the fork's change looks accidental, and list those for the owner. Fix the dead `README.md#state-and-plans` link in CONTRIBUTING.md. Branch: cloud/manual-ci-YYYY-MM-DD.
```
