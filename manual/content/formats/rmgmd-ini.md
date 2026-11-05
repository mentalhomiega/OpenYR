---
format_id: rmgmd-ini
title: RMGMD.INI
summary: Sets the random map generator's tiberium range, lighting, vegetation range and tree limit.
kind: file
filenames:
  - RMGMD.INI
key_scopes:
  - file: rmgmd.ini
    section:
      kind: literal
      name: General
related:
  - { type: system, id: map-generation }
  - { type: format, id: map-seed }
  - { type: format, id: mod-ini }
source_files:
  - code/rmgsettings.cpp
  - code/mapgen.cpp
---

`RMGMD.INI` holds the random map generator's settings, all in its `[General]` section. The generator reads the file each time it lays out a map, for a dialog preview and for a map that is played, and the map generator dialog reads it again each time its randomize button is pressed. The file is found through the ordinary file search, so a mod folder's copy takes precedence over the game's. When no copy is found, every setting keeps its default.

```ini title="RMGMD.INI"
[General]
RMGMinimumTiberium=900
RMGMaximumTiberium=1050
; morning, day, dusk, night
RMGLevelLightSettings=3,3,3,3
TemperateAmbientLight=75,100,75,35
TemperateAmbientRed=109,108,71,51
TemperateAmbientGreen=80,94,81,46
TemperateAmbientBlue=49,68,119,141
; archipelago, continent, team continent, inland, mountainous
RMGVegetationMinimums=60,60,60,60,60
RMGVegetationMaximums=100,100,100,100,100
MaxTrees=600
```

A list is whole numbers separated by commas. The lighting lists hold one entry per time of day, in the order morning, day, dusk and night, and the vegetation lists hold one entry per map type, in the order archipelago, continent, team continent, inland and mountainous. A list with fewer entries sets only those, and the rest keep their defaults. Entries past the last time of day or map type are ignored. The default of every list is the stock file's list.

A map's lighting comes from the snow lists on a tundra or taiga map, which uses the snow theater, and from the temperate lists on the other three biomes. [Random map generation](/systems/map-generation/) explains how each setting is used.

The stock file also lists `TemperateOrePatchLamps` and `SnowOrePatchLamps`. Neither has any effect: a generated map's only lamps are the `GALITE` floodlights around the start points.
