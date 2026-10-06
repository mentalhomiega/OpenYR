---
format_id: map-packs
title: Map packs
summary: Holds multiplayer maps and the scenario packet that lists them in one MIX archive.
kind: file
source_files:
- code/session.cpp
- code/mixfile.cpp
filenames:
- "*.YRO"
related:
- type: format
  id: mix
- type: key
  id: Description
  scope: map-packets
- type: key
  id: DescriptionText
  scope: map-packets
---

A map pack is a [MIX archive](/formats/mix/) with the extension `.YRO` that holds multiplayer maps and a scenario packet listing them. The packet has the pack's name with the extension `.PKT`, so `MyMaps.YRO` holds `MyMaps.PKT`. A pack named `MISSIONS.YRO` is never read.

Place the pack on its own in one of the folders the game searches (see [Game data](/using/game-data/)). The packet has the layout of any other scenario packet: its `[MultiMaps]` entries name sections, and each section describes one map, as the [`Description`](/keys/description/#scope-map-packets) page shows. The map file is the section name with `.MAP` appended, and it can be a member of the pack.

```ini title="MyMaps.PKT, inside MyMaps.YRO"
[MultiMaps]
1=MYMAP

[MYMAP] ; the map file is MYMAP.MAP
DescriptionText=Four player canyon
MinPlayers=2
MaxPlayers=4
```

## Listing

Each time the game builds the multiplayer map list, it mounts every pack it finds and adds the maps each packet lists. The packs' maps follow the maps of every `.PKT` file found on its own and come before the loose `.YRM` and `.MPR` maps. Packs are taken in alphabetical order of file name, and each pack's maps keep the order of its `[MultiMaps]` list. When no file of the packet's name is found, the pack adds no maps.

A pack's map shows its player limits after its row text, as in `Four player canyon (2-4)` for the example above. When [`MinPlayers`](/keys/minplayers/#scope-map-packets) and [`MaxPlayers`](/keys/maxplayers/#scope-map-packets) are equal, the row shows one number, as in `Four player canyon (4)`. The row keeps at most 43 bytes, so a long name loses some or all of its limits.

A match started from a spawn file does not build the map list, so it cannot play a map that only a pack holds.

## Mounting

A pack is mounted the first time the list is built after the game finds it, and it stays mounted until the game exits. Its members are searched after every archive in the [startup list](/formats/mix/#mounting-and-search-order), and a loose file of the same name is read in place of a member.

In a network game, a map listed by a pack counts as one that shipped with the game. A guest without the same map cannot download it from the host; the guest shows a message that it cannot play the map and leaves the game.
