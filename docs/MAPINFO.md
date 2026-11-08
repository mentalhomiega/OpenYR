# Map lister

`mapinfo` prints what a Red Alert 2 or Yuri's Revenge map contains: its houses,
triggers with their events and actions, tags, team types, scripts, task forces,
starting objects, waypoints and variables. It reads a loose map file and changes
nothing. Use it to check a mission's scripts without opening the editor, and in
tests that need a map's contents.

## Running it

```bash
mapinfo ALL02S.MAP
mapinfo --find "2 Extra Rocketeers" ALL02S.MAP
mapinfo --json ALL02S.MAP > all02s.json
```

| Option | Effect |
| --- | --- |
| `--find <text>` | Keeps the entries that mention the text: a type, tag, trigger name or ID, or house. The match ignores case and checks substrings. Each section header then reads `N of M match`. |
| `--json` | Writes the same entries as one JSON object. Section keys are listed under [JSON](#json). |
| `--help` | Prints the usage and exits with 0. |

The exit code is 0 when the map is read, 2 for a bad command line, a file that
cannot be opened, or a file with no INI sections.

The CMake build places `mapinfo` under `tools-bin/`, as it does `inicheck`.

### Getting a campaign map

Campaign maps are inside the game's MIX archives. `scripts/mapinfo.ps1` extracts
one and runs the lister on it:

```powershell
scripts\mapinfo.ps1 -Map ALL02S.MAP -Build agent26 -Find "Rocketeers"
```

The script looks for `dump-<Map>` in the build's `bin\Release` folder. If it is
not there, the script runs the game once through `scripts\run_autotest.ps1` with a
`dumpfile` step and `quit`. It then copies the dump to `build-<Build>\mapinfo\`
and reads that copy. The game runs hidden, as every autotest run does.

## Output

Plain text lists these sections in this order. Each section has a count. Triggers,
tags, team types, scripts, task forces and the tag index are sorted by ID; objects
are sorted by key; waypoints and variables by number.

| Section | Contents |
| --- | --- |
| Houses | The `[Houses]` list, with the player's house marked. |
| House numbers | The `[Countries]` list. House numbers in events and actions are positions in this list. |
| Triggers | ID, name, house, linked trigger, the disabled flag, the difficulties it is enabled on, its tags, then each event and action. |
| Tags | ID, name, repeat, and the trigger the tag fires. |
| Team types | Name, house, script, task force, max and priority. |
| Script types | Each numbered step as a team mission, with its decoded value. |
| Task forces | Each member as a count and a type. |
| Objects by tag | The reverse index: each tag an object carries, with the objects that carry it. |
| Waypoints | Each waypoint the map places, with its cell. |
| Variables | Each name in `[VariableNames]`. |
| Starting units, infantry, aircraft, structures | Owner, type, cell, health, mission and tag. |

A trigger's repeat type comes from its tag. The engine has no repeat setting on a
trigger itself, so each tag line shows the repeat that applies when the trigger
springs.

## Names and parameters

Event, action and team mission names are the enumerator names in
`code/tevent.hh`, `code/taction.hh` and `code/tmission.hh`, without their prefix.
The parameter kind of each event, action and mission comes from the engine's
`Event_Needs`, `Action_Needs` and `TeamMission_Needs`. `tools/mapinfo/gen_tables.py`
reads those sources and writes `tools/mapinfo/tables.inc`. When the engine adds or
changes an enumerator, rerun the script and commit the result. The
`mapinfo-tables-current` test fails while the file is stale.

A parameter prints under the key given in this table.

| Map field | Printed as |
| --- | --- |
| Event parameter code 1, or a team in a team mission | `team=<name> (<ID>)` |
| Action parameter code 2 | `trigger=<name> (<ID>)` |
| Action parameter code 3 | `tag=<name> (<ID>)` |
| Action parameter code 4 | `text=<CSF label>` |
| Action parameter code 6, 7, 8 | `eva=`, `sound=` or `theme=`, with the name as written |
| Action parameter code 9, 10 | `type=` with the ID as written, and `extra=` for the trailing number |
| Action parameter code 11 | `super=` and `extra=` |
| A house-kind number | `house=<name>`, read through `[Countries]` |
| A waypoint-kind number or trailing field | `waypoint=<letter> cell (x,y)`, or `(not placed)` |
| A variable-kind number | `global=` or `local=`, with the name from `[VariableNames]` |
| A structure, unit, infantry or aircraft type number | `type-number=` with the number |
| Any other number | `value=` |

An action's trailing waypoint is printed whenever the map gives a letter there.
Some actions store `A` even when they do not use a waypoint, so their output shows
`waypoint=A` too.

A trigger's disabled flag is its fourth field. The engine treats the trigger as
enabled when that field is 0.

## Limits

- House numbers resolve through the map's `[Countries]` list. We checked this on
  Red Alert 2 maps only. For a Yuri's Revenge map that has no `[Countries]`, a
  house number prints as `house <n>`.
- Type numbers, quarry types, movies, animations and sound effects with a
  location print as the number or name the map holds. The lister does not map a
  type number to a type name, because that needs the rules list the map is played
  with.
- A map's `[Basic] NewINIFormat` decides how object cells are read, as the engine
  does: a pair of numbers from format 4, or one number counted in rows of 128
  before it.

## JSON

The top-level object holds a `map` object, then one array per section, with keys
`houses`, `countries`, `triggers`, `tags`, `teams`, `scripts`, `taskForces`,
`tagCarriers`, `waypoints`, `variables`, `units`, `infantry`, `aircraft` and
`structures`. A `--find` run keeps the same keys and filters the arrays. Trigger
fields hold the raw ID, such as `id`, `linked`, `tag` and `script`. Each command's
parameters are an object whose keys are listed above, with values as printed in the text output.

## Tests

`tests/mapinfo/sample.map` is a hand-written map of a few lines. It holds one
trigger with a Discovered event, Change House, sound and trigger actions, a tag
that a unit and an infantryman carry, a team, a script and a task force. The
`mapinfo-sample` test checks the decoded fields in the JSON output and the
`--find` counts. It needs Python 3, and the test is not added without it.
`mapinfo-tables-current` checks that `tables.inc` matches the engine sources.
