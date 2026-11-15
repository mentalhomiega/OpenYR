---
format_id: mod-ini
title: mod.ini
summary: Names a mod, the overlay files it reads over the rules, art and AI files, and the base files and archives it replaces.
kind: file
source_files:
  - code/mods.cpp
  - code/mods.h
  - code/deploymentconfig.cpp
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

`mod.ini` describes a [mod](/using/mods/) and sits in the mod's folder. The file is optional: a mod without it has no overlays, replaces no base files and takes its folder's name. The game reads `mod.ini` from the mod's folder only, never from an archive or another folder.

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

## Replacing base files and archives

A mod that is a complete conversion, rather than a set of changes, can name its own copies of the files the game reads and the archives it mounts. `[Files]` and `[Palettes]` use the key names of the same sections in [`OPENTS.INI`](/formats/opents-ini/#the-files-it-reads), and `[Archives]` adds the archive prefixes.

```ini title="mod.ini"
[Files]
Rules=RULESCV.INI
Art=ARTCV.INI
AI=AICV.INI
Sound=SOUNDCV.INI
Theme=THEMECV.INI
Eva=EVACV.INI
Battle=BATTLECV.INI

[Archives]
ExpandPrefix=EXPANDCV,EXPANDMD
CachePrefix=ECACHE
```

| Key | Meaning |
| --- | --- |
| `[Files]` `Rules`, `Art`, `RA2Art`, `AI`, `Sound`, `Theme`, `Eva`, `Battle`, `LanguageRules`, `MultiplayerRules`, `Tutorial`, `UI`, `Keyboard`, `Mission`, `MapSelect`, and the `Expansion` keys | The name of the file the game reads in place of the deployment's. The game looks for it in the usual [search order](/formats/opents-ini/#the-order-files-are-searched-for-in), so the file may sit in the mod's folder or in one of its archives. |
| `[Files]` `TheaterSuffix` | The suffix of each theater's tile set file. `MO` reads `TEMPERATMO.INI` in place of `TEMPERATMD.INI`, and a theater without such a file falls back to its `MD` file. |
| `[Palettes]` `Scheme`, `Game` | The palettes in force until a theater is loaded |
| `[Archives]` `ExpandPrefix` | Comma separated prefixes of the numbered expansion archives. `EXPANDCV` mounts `EXPANDCV99.MIX` down to `EXPANDCV00.MIX`, and the archives of an earlier prefix are searched before those of a later one. The default is `EXPANDMD`. |
| `[Archives]` `CachePrefix` | The same for the cached archives. The default is `ECACHE`. |

`[Menu]` `FollowMenuButtons=no` keeps every title menu button although the mod's UI file hides, disables or redirects some in its `[UISettings]` section. The default is `yes`: the menus follow the UI file. A mod written for a launcher that supplies its own menus often hides the Single Player and Skirmish buttons, and this key keeps them.

A key the mod leaves out keeps the value the deployment or an earlier mod gave, and where two mods write the same key the later mod's value is used. These keys replace a file's name, so they differ from `Rules=`, `Art=` and `AI=` under `[Mod]`, which name an overlay that is read over the base file. `Settings` cannot be set here: the player's settings file is read before any mod is installed.

While a mod is active, the game mounts the patch and expansion archives ahead of the language archives. `RA2MD.CSF`, and any other file the language archives hold, can then come from the mod's `EXPANDCV` archive. Without a mod the language archives stay first. A file the game reads into memory, such as a shape or a palette, is read from a mod's expansion archive on first use and kept until the game exits, so a mod's large expansion archives need not be cached.

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

## Game modes and game options

A mod can give the skirmish lobby its own game modes and game options, declared in the files of the CnCNet client the mod is played through. `[Lobby]` names those files, relative to the mod's folder, and the paths inside them are taken from the same folder.

```ini title="mod.ini"
[Lobby]
Maps=INI\MentalOmegaMaps.ini
Options=Resources\GameOptions.ini
CustomMapModes=Standard,No Bases
```

| Key | Meaning |
| --- | --- |
| `Maps` | The client's map list file: its `[GameModes]`, one section per mode, `[MultiMaps]` and one section per map |
| `Options` | The client's game options file, whose `[SkirmishLobby]` lists the check boxes and drop-downs the lobby shows |
| `CustomMapModes` | The modes that also offer maps the map list does not name, such as the game's own maps. Without it, such maps are offered in no mode. |

The skirmish lobby then shows a game mode list and the declared options in place of its own switches and sliders, and lists only the maps the chosen mode offers. When several mods name `Maps`, the last one is used. A game in which no mod names `Maps` keeps the lobby as it was. How the game reads the declarations and applies them to a match is described in `docs/LOBBY_MODES.md` in the source tree.
