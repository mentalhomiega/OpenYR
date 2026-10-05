---
title: Mods
summary: A mod is a folder of game files that the game searches ahead of its own, with optional overlays for the rules, art and AI files.
category: configuration
source_files:
  - code/mods.cpp
  - code/cdfile.cpp
  - code/gamedirs.cpp
  - code/init.cpp
  - code/startup.cpp
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

List mods in `Mods=` in [`OPENTS.INI`](/formats/opents-ini/#the-mods-it-starts-with), or name each one with [`-MOD=`](/using/command-line/mod/) on the command line:

```text
Game.exe -MOD=HighTech -MOD="D:\Mods\Map Pack"
```

The game takes the mods `OPENTS.INI` lists first, then the command line's mods in the order given, and reads them in that order. Where two mods hold the same file or write the same key, the later mod's version is used. The mods are chosen at startup and stay in force until the game exits.

A name is a folder inside the `Mods` folder of the game data directory, so `HighTech` is `Mods\HighTech\`. The game data directory is the one [`-DATADIR`](/using/command-line/data-directory/) names, or the game's own directory. A path that starts with a drive letter or a backslash names the folder directly.

- A mod whose folder does not exist is skipped, and the debug log names the folder it looked for.
- A folder named twice is used once, in the position where it was first named.

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

## Limits

- The game does not check that players run the same mods. Give every player in a multiplayer game the same mods in the same order; a game between players whose mods differ can go out of sync.
- A saved game does not record the mods. Load a save with the mods it was played with.
