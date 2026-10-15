# Key index log, 2026-10-15

Branch `cloud/keys-index-2026-10-15`, begun from `origin/modern` at 4d36e67d. The worktree's own HEAD (e1727a43) shares no history with `origin/modern`, so I branched from `origin/modern` as the briefing says.

## Commits

| Commit | What it does |
| --- | --- |
| Regenerate the key catalog with the structure recoil keys | `manage.py update` added the nine recoil keys (`TurretRecoil`, `TurretTravel`, `BarrelTravel` and the six frame keys) to `manual/data/ini-keys.yaml`, which had missed them. Nothing else changed. |
| Add a script that indexes keys gamemd does not read | `manual/tools/openyr_keys.py` writes `docs/OPENYR_KEYS.md`; `manual/tools/openyr_keys.yaml` holds the decisions and the SPAWN.INI rows; `manual/tools/tests/test_openyr_keys.py` has ten tests on a synthetic tree. |
| Add the index of OpenYR-only INI keys | The generated `docs/OPENYR_KEYS.md` (rules, art, client settings, UI.INI, other game files, SPAWN.INI, OPENTS.INI and the launcher), linked from `docs/README.md`. |
| Describe the key index in the manual guides | `manual/AUTHORING.md` tells authors to classify a key-adding record; `manual/MAINTAINING.md` lists the new authority. |

## How the index is built

- Rules, art, client settings, UI.INI and the other game files: the script takes every change record that adds a key, drops the records classified as gamemd's, and reads each remaining key's section, default and summary from `ini-keys.yaml` and the key page.
- A record is gamemd's when its name starts with `yr-` or `openyr_keys.yaml` lists it under `gamemd_records`. 142 records without that prefix add keys; the file classifies all of them: 14 as OpenYR, 27 as OpenTS, 11 as Ares or Phobos, 82 as gamemd, 8 as undecided.
- SPAWN.INI has no catalog. Its 64 rows are written in `openyr_keys.yaml`; the script checks that each key appears as a quoted name in the launch file code and that each anchor is a heading of the launch file page.
- OPENTS.INI: the script reads the key names from `code/deploymentconfig.cpp` and the defaults from `code/deploymentconfig.h`; the effect lines are in `openyr_keys.yaml`. The launcher's `[Update]` rows come from the table in `docs/LAUNCHER.md`.
- `--check` fails when the page is stale or a key-adding record has no decision. `--report` lists records that need one.

## Findings

- `ColorRGB` is not in this tree. `code/spawnerconfig.cpp` reads no such key, and `RemapColorRGB` is a house field. The index lists the 64 launch file rows the code does read.
- The change record `building-turret-recoil` says structures recoil "only when their rules set `TurretRecoil=yes`, as in Yuri's Revenge", so I classified it as gamemd's and the recoil keys are not in the index. The job named them as OpenYR-only. The record lists three of the nine recoil keys as targets; the six frame keys have pages but no record target.
- 142 records is too many to check against the original game from here. I decided each from the record's own text and what I know of the stock rules, and I could not check any against gamemd. The 82 gamemd and the 8 undecided entries matter most: a wrong entry hides a key or lists one gamemd reads. The undecided records are `healer-target-kinds`, `more-shape-facings`, `pip-row-length`, `produce-cash-buildings`, `spawn-house-base-nodes`, `theaters-declared-in-rules`, `totable` and `vehicle-transports`.
- `DescriptionText` in map packets is listed as OpenYR on judgment. The page does not say whether Yuri's Revenge reads it.
- Several SPAWN.INI effects are short because the launch file page states the option without explaining it (`Bases`, `Crates`, `FogOfWar`). I wrote them from the option names in `code/spawner.cpp` and the page, not from a trace of each.
- Keys with a page but no `added` target are invisible to the script. The six recoil frame keys are the case I found.

## Ran

- `python manual/tools/manage.py update`: verified 2,339 keys and the lifecycle; it regenerated `ini-keys.yaml` (committed) and left `commands.yaml` and `scripting.yaml` unchanged.
- `python manual/tools/openyr_keys.py` and `--check`: the page is current. `--report`: no unclassified records.
- `python -m unittest discover -s manual/tools/tests`: 138 tests, all pass, including the ten new ones.
- `node manual/site/scripts/check-doc-links.mjs`: all internal links and anchors in 4,083 Markdown files resolve, including every link in `docs/OPENYR_KEYS.md`.

## Not run

- The full `manage.py check`: the site dependencies are missing here, so the site build and the render, search and node tests did not run.
- Any build, and the game. This branch changes no code.
- `pip install -r manual/tools/requirements.txt` failed on the Debian PyYAML; the tools ran with the installed PyYAML 6.0.1.

## Next

- Add `openyr_keys.py --check` to the manual CI, so a record that adds a key cannot merge without a decision. I did not wire it into `manage.py check`.
- Settle the eight undecided records, then the recoil question below.
- Give the six recoil frame keys a target in `building-turret-recoil`, or a record of their own.

## Questions for MentalHomiega

- Does Yuri's Revenge read `TurretRecoil`, `TurretTravel`, `BarrelTravel` and the six frame keys? If not, move `building-turret-recoil` from `gamemd_records` to `records` in `manual/tools/openyr_keys.yaml`.
- Which of the eight undecided records add keys that gamemd lacks?
- Is `DescriptionText` your own key?
- Where does `ColorRGB` live? It may be on a branch that is not in `origin/modern`.
