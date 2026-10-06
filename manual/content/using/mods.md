---
title: Mods
summary: A mod is a folder of game files that the game searches ahead of its own, with optional overlays for the rules, art and AI files.
category: configuration
source_files:
  - code/mods.cpp
  - code/modchoice.cpp
  - code/cdfile.cpp
  - code/deploymentconfig.cpp
  - code/gamedirs.cpp
  - code/init.cpp
  - code/saveload.cpp
  - code/startup.cpp
  - code/ui/screens/mods/uimods.cpp
  - code/ui/screens/mods/uimodsdlg.cpp
related:
  - type: format
    id: mod-ini
  - type: format
    id: opents-ini
  - type: command
    id: launch:mod
  - type: using
    id: game-data
---

A mod is a folder of files that the game reads ahead of its own. A file in a mod's folder replaces the game's file of the same name, whether the game's copy is loose or inside an archive, except for the data the game reads only from cached archives. A mod can also carry a [`mod.ini`](/formats/mod-ini/) that names overlays: INI files read over the rules, art and AI files, which change only the keys they write.

## Choosing mods

Choose mods on the [Mods screen](#the-mods-screen), list them in [`Mods=` in `RA2MD.INI`](/keys/mods/) or [in `OPENTS.INI`](/formats/opents-ini/#the-mods-it-starts-with), or name each one with [`-MOD=`](/using/command-line/mod/) on the command line:

```text
Game.exe -MOD=HighTech -MOD="D:\Mods\Map Pack"
```

The game takes the mods the list names first, then the command line's mods in the order given, and reads them in that order. The list is the player's `Mods=` under `[Options]` in `RA2MD.INI` when the file has that key, and otherwise the `Mods=` under `[Paths]` in `OPENTS.INI`, which a deployment can ship as a default. A player's list replaces the deployment's; it does not add to it, and `Mods=,` in `RA2MD.INI` turns off every mod the deployment lists. Where two mods hold the same file or write the same key, the later mod's version is used. The mods are chosen at startup and stay in force until the game exits.

A name is a folder inside the `Mods` folder of the game data directory, so `HighTech` is `Mods\HighTech\`. The game data directory is the one [`-DATADIR`](/using/command-line/data-directory/) names, or the game's own directory. A path that starts with a drive letter or a backslash names the folder directly.

- A mod whose folder does not exist is skipped, and the debug log names the folder it looked for.
- A folder named twice is used once, in the position where it was first named.

### The Mods screen

**Options**, then **Mods**, on the main menu opens a list of every folder in the `Mods` folder of the game data directory, together with the mods the list names and the mods the command line names. Each row shows the mod's name from its [`mod.ini`](/formats/mod-ini/), or its folder's name, and selecting a row shows the mod's description and folder. A ticked mod is on. The mods the game can read are numbered in the order it reads them, so each mod overrides the ones above it.

- Tick a mod, or select it and choose **Turn On**, to add it to the end of the list. Untick it, or choose **Turn Off**, to take it out.
- **Move Up** and **Move Down** move the selected mod within the list.
- **OK** writes a changed list and shows a message that the change takes effect after a restart. The mods in force stay the same until the game starts again. **Cancel** discards the changes.

A mod only the command line names is marked *command line*. It stays ticked, cannot be moved, and is never written to the list, so it is on only while the game is started with its `-MOD=`. A listed mod that the command line also names keeps its listed place; turning it off moves it below the listed mods, where the command line puts it.

A listed mod whose folder does not exist is marked *not found* and has no number. It stays in the list until it is turned off. A folder whose name holds a comma or a semicolon, or starts or ends with a space, cannot be turned on, because the list cannot hold that name; rename the folder to use it.

OK writes the list as `Mods=` under `[Options]` in the player's `RA2MD.INI`, which is in the [user data directory](/using/game-data/#keeping-the-data-somewhere-else) when [`-USERDIR`](/using/command-line/user-directory/) names one. The file's other settings are kept, and the game data directory is never written to, so players who share one data directory keep separate lists. Turning every mod off writes `Mods=,` rather than removing the key, so the choice overrides a list in `OPENTS.INI`. If the file cannot be written, the screen stays open and names the file.

## What a mod folder holds

A mod folder can hold any file the game looks for in its search order, such as INI files, maps and archives. The game looks for a file in the player's own directory first, then in the mods' folders, last mod first, then in its own directories; [`OPENTS.INI`](/formats/opents-ini/#the-order-files-are-searched-for-in) gives the full order. A search by pattern, such as the one that lists maps, also covers the mods' folders.

Data the game reads only from a cached archive, such as palettes and many shapes, is not replaced by a loose file in a mod folder. Ship such files in an `ECACHE` archive in the mod's folder instead; [MIX archives](/formats/mix/#caching) covers which files this applies to. A mod's archive replaces any archive of the same name, so give a mod's `ECACHE` and `EXPANDMD` archives numbers the game and the other mods do not use.

PNG sprite sheets are found like any other file, so a mod can add a sprite sheet for a new type or replace the frames of an existing shape.

To replace a whole file, put a file with its name in the mod's folder. To change some keys of the rules, art or AI file, use an overlay, which keeps the rest of the file and any other mod's changes.

:::caution[Do not name an overlay after a file the game reads]
A mod's folder is searched for every file, so an overlay named `RULESMD.INI` also replaces the game's whole rules file. Give each overlay a name the game does not read, such as `rules.ini` when the rules file is `RULESMD.INI`.
:::

## Checking which mods are active

At startup the debug log lists each active mod with its folder and overlays, then the number of active mods. Each overlay is logged again when it is read:

```text
[Mods] Mod 1: High Tech, folder D:\Game\Mods\HighTech\
[Mods]   rules overlay D:\Game\Mods\HighTech\rules.ini
[Mods] 1 mods active.
[Mods] High Tech: read D:\Game\Mods\HighTech\rules.ini.
```

## Saved games

A saved game records the folder names of the mods that were active when it was saved. A save made under different mods still loads, but its rules come from the save while its art and every other file come from the mods active now, so load a save with the mods it was played with. When the lists differ, the message list shows both after the load and the debug log records them:

```text
[Mods] The save was made with mods "HighTech"; the mods in force are "".
```

The folder names are compared without regard to case. A save from a build that did not record mods loads without the comparison.

## Limits

- The game does not check that players run the same mods. Give every player in a multiplayer game the same mods in the same order; a game between players whose mods differ can go out of sync.
