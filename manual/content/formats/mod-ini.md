---
format_id: mod-ini
title: mod.ini
summary: Names a mod and the overlay files it reads over the rules, art and AI files.
kind: file
source_files:
  - code/mods.cpp
  - code/mods.h
  - code/init.cpp
  - code/rules.cpp
  - code/rulesreload.cpp
  - code/rulescheck.cpp
filenames:
  - mod.ini
related:
  - type: using
    id: mods
  - type: format
    id: opents-ini
  - type: format
    id: ini-syntax
  - type: command
    id: ReloadRules
  - type: command
    id: launch:mod
---

`mod.ini` describes a [mod](/using/mods/) and sits in the mod's folder. The file is optional: a mod without it has no overlays and takes its folder's name. The game reads `mod.ini` from the mod's folder only, never from an archive or another folder.

```ini title="mod.ini"
[Mod]
Name=High Tech
Description=Adds a laser tank and makes the Grizzly cheaper.
Rules=rules.ini
Art=art.ini
AI=ai.ini
```

| Key | Meaning |
| --- | --- |
| `Name` | The name the debug log and the [Mods screen](/using/mods/#the-mods-screen) give the mod. Without it, the name of the mod's folder. |
| `Description` | A description of the mod, which the Mods screen shows for the selected mod. |
| `Rules` | The rules overlay |
| `Art` | The art overlay |
| `AI` | The AI overlay |

Each overlay key names a file relative to the mod's folder, such as `rules.ini` or `INI\rules.ini`. The game opens the overlay at that path only. If no file is there, the debug log says so and the game reads nothing in its place: a file of the same name in another folder or in an archive is never read instead.

## How overlays are read

An overlay is read into the same database as the file it changes. A key it writes replaces the earlier value, a section or key it adds is added, and everything else keeps its earlier value. [INI syntax](/formats/ini-syntax/#repeats-and-later-files) covers how a replaced key moves within its section, which matters for type lists.

| Overlay | Read over | Read |
| --- | --- | --- |
| Rules | The rules file | At startup, and again by [Reload rules](/commands/reloadrules/) |
| Art | The art file, and the expansion's art file when Firestorm is enabled | At startup, when a game starts or a saved game loads, and by Reload rules |
| AI | The AI file | At startup |

The mods' overlays of one kind are read in mod order, so where two mods write the same key, the later mod's value is used.

The rules overlays sit directly above the rules file. Every rules layer the game reads later still overrides them: the translated rules, the expansion's rules when Firestorm is enabled, the [multiplayer rules](/formats/multiplayer-rules/) outside a campaign, and the map's own rule overrides.

A type that a rules overlay adds to a type list, such as `[InfantryTypes]`, is created when a game starts, like a type the rules file lists.

## Checking overlays

At startup and after each Reload rules, the game checks each mod's rules and art overlays for keys the engine does not read and values it cannot parse, and writes each finding to the debug log as an `INI check:` line under the overlay's path. The check runs only when `inicheck-catalog.tsv` is beside the executable. A type section in a rules overlay is checked when the rules file, an earlier mod's rules overlay or the overlay itself lists the type.
