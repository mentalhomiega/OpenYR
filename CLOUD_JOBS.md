# Cloud jobs

Ready-made prompts for cloud sessions the owner starts by hand. A cloud session started from claude.ai/code (or the "Start a cloud session" button) on `mentalhomiega/OpenYR` uses the cloud session credits; routines do not.

Keep sessions cheap: choose Sonnet at default effort, and give each session one job with a clear end. A session told to keep going until a set time spends far more. The credits last until 5 November; about $7 a day uses them evenly.

To start one, open a new cloud session on the repository, paste the shared opening below, then one job underneath it. Several jobs can run at once in separate sessions; each pushes its own branch, and the morning test on the owner's PC picks them up.

## Shared opening

```text
You are working on the GitHub repository mentalhomiega/OpenYR, a fork of OpenTS that rebuilds the Red Alert 2: Yuri's Revenge engine. Run `git fetch origin` and `git checkout yr`, then read CLOUD_BRIEFING.md at the repository root and follow its hard rules, its night-log section and its report section. You cannot run the game and there are no game files; never add game assets, binaries or decompiled code. Never write the owner's real name anywhere; the owner is MentalHomiega, and commits use `MentalHomiega <182634060+mentalhomiega@users.noreply.github.com>` (set it with git config before committing). Commit in small commits with imperative subjects of at most 72 characters and no AI-attribution or Co-authored-by lines. Push only to the new branch named in the job below (add today's UTC date where it says YYYY-MM-DD), never to `yr` or `main`; never force-push, delete branches or open pull requests, issues or comments anywhere. Before finishing, add your log as `cloud-log/YYYY-MM-DD-<job>.md` on your branch, then end with the briefing's report.

Your job:
```

## Jobs

The seven jobs from 2026-10-04 were run once, and their branches are merged into `cloud/nightly-2026-10-05` and `cloud/nightly-2026-10-06`. The prompts below continue from them. A job that starts from a `cloud/*-2026-10-04` branch must begin from that branch merged with `yr`, because `yr` has moved on since.

In a manual page, link a key that has one scope as `/keys/<key>/` and a script line as `/mapping/missions/N/`; the links check fails on `/keys/<key>--<scope>/` and `/scripting/missions/N/`.

### Research: Ares and Phobos tables

```text
Follow docs/research/RESEARCH_BRIEFING.md. `yr` already holds the research up to `cloud/research-2026-10-04`; read its logs under docs/research/log/. Next: table the Phobos attached-effect types and shields; then laser trails, radiation types, and the terrain, ore, overlay, particle, country and voxel-animation sections; then the Ares bugfix pages and `whatsnew`, and regenerate docs/research/tools/data/ares.txt. If time is left, spot-check the effect descriptions of the first three ranked items in priorities.md against the Phobos source. Branch: cloud/research-YYYY-MM-DD, begun from `yr`.
```

### INI checker, next stage

```text
Continue the INI checker. `yr` holds the first version; `origin/cloud/inicheck-2026-10-04` adds numbered keys (`Weapon{1-18}`, `DockingOffset{0-}`) and follows `Primary=`, `Secondary=`, `Projectile=` and `Warhead=` into weapons, projectiles and warheads. It conflicts with `yr` in the checker files because `yr` carries the first version as a different commit, so begin from `yr` and re-apply that branch's two code commits, resolving the conflicts. Read docs/INICHECK.md and cloud-log/2026-10-04-inicheck.md on that branch. Then: place `art.ini` image sections, check difficulty sections, and support zero-padded numbered keys such as `Tile%02dAnim`. Keep it report-only, with tests for each addition. Do not call it from the game; the owner has not chosen where. Branch: cloud/inicheck-YYYY-MM-DD.
```

### Static analysis

```text
`cloud/static-2026-10-04` fixed the two defects cppcheck proved and listed the rest in cloud-log/2026-10-04-static.md. Work through that list: fix an uninitialised member only where the constructor leaves it unset on a path that reads it, one fix per commit, and leave the rest listed with file and line. Do not run clang-tidy; it needs the Windows build's compile database. Branch: cloud/static-YYYY-MM-DD, begun from `yr`.
```

### Unit tests

```text
`cloud/tests-2026-10-04` added tests for the radio slot rules (`code/radioslots.h`) and the NavalTargeting table (`code/navalweapon.h`). `TechnoClass::What_Weapon_Should_I_Use` needs too much engine state to test whole. Extract further self-contained rules into headers the same way, with no change in behaviour, and test them without game files: the mission numbering in `code/mission.hh` and `code/_mission.cpp`, and rules defaults. Use the existing test setup. If a function cannot be tested without large engine state, say so in the log instead of forcing it. Branch: cloud/tests-YYYY-MM-DD, begun from `yr`.
```

### Dock and radio follow-ups

```text
`cloud/docks-2026-10-04` fixed the carrier spawn leak, the untargeted `RADIO_TETHER` messages and the bump rule, and corrected the dock manual pages. The remaining dock findings are listed in cloud-log/2026-10-04-docks.md: code that serves only slot 0 at a multi-dock building. Do not change them until the owner says whether such a building should act on every dock; instead, write down for each what changing it would alter at a single-dock building, and fix only what leaves single-dock behaviour as it is. A `UnitReload` pad rearms and repairs. Each fix gets its own commit and its manual update. Branch: cloud/docks-YYYY-MM-DD, begun from `yr`.
```

### Mind-controlled units entering buildings

```text
The work is on `origin/cloud/mind-control-enter-2026-10-04`. The owner's answers: a mind-controlled unit may not board a transport, garrison a structure or enter a tank bunker; Grinders and Bio Reactors still take it; a controlled engineer may repair and capture and a controlled spy may infiltrate. Hospitals and armories are refused on that branch without the owner's confirmation. Open items in its log: a team waiting on `TMission_FULLY_LOADED` for controlled members, a computer house's controlled engineer picking structures it cannot enter, and a vehicle already in a tank bunker when it is controlled. Check each against the code, fix the ones that are plainly wrong with a change record, and list the rest. Branch: cloud/mind-control-enter-YYYY-MM-DD, begun from that branch merged with `yr`.
```

### Green manual checks on `yr`

```text
`origin/cloud/manual-ci-2026-10-04` updates the tests the fork changed on purpose and fixes the links and checks that failed after them. It conflicts with `yr` in manual/site/scripts/check-render.mjs. Begin from that branch merged with `yr`, resolve the conflict, then run `python manual/tools/manage.py check` and the site build with its render, search and link checks (`npm ci` in manual/site first). Fix what still fails: update a test to the fork's intended behaviour and say why in the log; restore the pinned behaviour only where the fork's change looks accidental, and list those for the owner. Branch: cloud/manual-ci-YYYY-MM-DD.
```
