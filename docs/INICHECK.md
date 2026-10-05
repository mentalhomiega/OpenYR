# INI checker

`inicheck` reads a rules file and lists the keys the engine does not read there
and the values it cannot read in the expected form. It only reports. Neither
the tool nor its library changes how the game reads a value.

## In the game

On the `modern` branch the game checks its rules file (`rulesmd.ini`) and art file
(`artmd.ini`) each time it loads the rules at startup, and each map's own keys and rule
overrides when the map loads. The findings go to the debug log as `INI check:` lines: the first
200 for each file, then its totals. The check needs `inicheck-catalog.tsv` beside the
executable; the build writes it there when Python 3 with PyYAML is installed. Without the
catalog the log says the file was not checked, and the game runs as usual.

An art section is checked when a type in the rules names it with `Image=`, or has its name and
no `Image=`. A map's type sections are placed through the rules lists in the map or the rules
file, and its house, team and task force sections through the map's `[Houses]`, `[TeamTypes]`
and `[TaskForces]` lists.

Stock Yuri's Revenge files give about 900 findings in the rules and 1,200 in the art, most of
them keys the engine does not read yet, so the lists double as a to-do list for engine work.

## Running it

Write the key catalog from the manual's generated key list, then check a file:

```bash
python tools/inicheck/export_catalog.py --output inicheck-catalog.tsv
inicheck --catalog inicheck-catalog.tsv rulesmd.ini
```

The export script needs PyYAML, which `manual/tools/requirements.txt` already
installs. The CMake build places `inicheck` under `tools-bin/` in the build
directory.

| Option | Effect |
| --- | --- |
| `--catalog <file>` | The catalog to check against. Required. |
| `--file <name>` | Which file's keys to use, as the catalog names it. The default is `rules.ini`; use it for `rulesmd.ini` too. |
| `--unchecked` | Also lists each section the checker skipped. |

Each finding is one line naming the file, line, section, key and value. The
exit code is 0 when there are no findings, 1 when there are, and 2 when the
catalog or the file cannot be read.

## What it reports

- A key the engine does not read in that section, such as a misspelling, a key
  of another section, or a key the engine has not ported yet.
- A key whose spelling matches a read key only when case is ignored. The engine
  matches section and key names exactly, so `speed=` is not `Speed=`.
- A value the engine would not read as the key's form. The forms checked are
  yes/no, whole numbers, numbers, two- and three-number points, colors and
  lists of whole numbers. A yes/no value counts by its first letter, and a
  whole number may be written in hexadecimal with a leading `$` or a trailing
  `h`, as the engine reads them. A three-number point accepts fractions
  whether the engine reads it as whole numbers or not. Values that name other
  types or sounds are not checked.

## What it checks

A section is checked when the catalog names it, such as `[General]` or
`[AI]`, or when a rules list such as `[VehicleTypes]` or `[Warheads]` names it.
A listed section is checked against the keys of that type kind, so a vehicle
section accepts vehicle keys and the keys every object type reads.

Every other section is skipped. That includes weapons and projectiles, which
no list names, and sections such as difficulty settings whose names the
catalog does not give.

## Limits

The catalog holds the keys the manual's extractor finds in the source. A key
the engine builds from a pattern, such as `DockingOffset0=`, may be missing
from it and is then reported although the engine reads it. Treat a finding as
something to look at, not as proof of an error.

## The catalog format

`export_catalog.py` writes one line per place the engine reads a key, as five
tab-separated fields: the key, the file, the section, the type kinds and the
value type. The section is the literal section name, `*` for an object
section, or `@` followed by the section source for a section the checker does
not place. Lines starting with `#` are skipped. Do not edit a written catalog;
export it again after `manual/data/ini-keys.yaml` changes.

## Tests

`tests/inicheck` pins the library with a small inline catalog and needs no
game data. The `inicheck-cli` test runs the tool on the sample in
`tools/inicheck/`.
