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

The seven jobs of 4 October (research, INI checker, static analysis, unit tests, docks and radio, mind-controlled units entering buildings, green manual checks) have run; their branches wait for the owner's review, so do not repeat them. The jobs below follow on from them or cover work added to `yr` since. Jobs marked "needs `yr` pushed" read commits that must be on `origin/yr` first (they are, once `yr` is past `f3c7520`).

### Research: Phobos attached effects and shields

```text
Follow docs/research/RESEARCH_BRIEFING.md. Continue from the 2026-10-04 research log (`cloud-log/2026-10-04-research.md`, now on `yr`) at its "Next session" list: table the Phobos attached-effect types and shields first, then the remaining Phobos sections it lists, then the Ares bugfix pages and `whatsnew`, regenerating `tools/data/ares.txt` once the Ares table is complete. Stop after the attached effects and shields if the session runs long, and say where you stopped. Branch: cloud/research-YYYY-MM-DD.
```

### INI checker, stage three

```text
Continue the INI checker from `origin/cloud/inicheck-2026-10-04` merged with `yr` (read docs/INICHECK.md and `cloud-log/2026-10-04-inicheck.md` on that branch). Do its "Open" items: support zero-padded numbered keys such as `Tile%02dAnim` and `Territory%02d` in the catalog's range syntax and the checker, and place `art.ini` image sections so the art patterns apply. Keep it report-only, with tests for each addition. Leave where the game calls the checker to the owner. Branch: cloud/inicheck-YYYY-MM-DD.
```

### Static analysis follow-up

```text
Start from `origin/cloud/static-2026-10-04` merged with `yr` and read `cloud-log/2026-10-04-static.md`. Go through its "Remaining findings" list (skip `duplInheritedMember`): for each, read the code and decide whether the code itself proves it wrong. Fix only those, one fix per commit, keeping save-game layouts and simulation unchanged; for the rest, give a one-line reason in your log. Branch: cloud/static-YYYY-MM-DD.
```

### Review of the new `yr` fixes (needs `yr` pushed)

```text
Review the `yr` commits after `7832f3a` (run `git log --oneline 7832f3a..origin/yr`): theater tile sets from `<Root>MD.INI`, 702 waypoints, the Yuri's Revenge multiplayer map list (MISSIONSMD.PKT, `.YRO` packs, loose `.YRM` maps), campaign names, mission briefings from MISSIONMD.INI, map objects owned by a playing country in multiplayer, and the gap generator exit hang. Look for bugs, unhandled cases (missing files, empty sections, long names, a comma in a map name), buffer sizes, and claims in comments or the manual that the code does not support. Fix what the code proves wrong, one commit each; list the rest with file and line. Branch: cloud/review-YYYY-MM-DD.
```

### Unit tests for the map list and theater readers (needs `yr` pushed)

```text
Add unit tests that need no game files for the code the new map list uses: reading a `.YRM` map's `[Basic]` section (Name, missing Name gives "No Name", player limits), a packet entry's `DescriptionText=` against `Description=` labels, the " (2-4)" / " (2)" player-limit suffix for maps from a `.YRO` pack, and the theater control file name (`<Root>MD.INI`, falling back to `<Root>.INI`). Use the existing test setup; if a function needs too much engine state, test the smallest piece you can lift out without changing behaviour, or say so in the log. Branch: cloud/tests-YYYY-MM-DD.
```
