---
title: Build next to an ally's base only where its type allows it
category: feature
release: 0.2.0
breaking: true
migration:
- '`BuildOffAllyAnyStructure=` under `[MultiplayerDefaults]` in `rules.ini` is no longer read. Remove it or leave it; it has no effect.'
- Add `EligibileForAllyBuilding=yes` to the section of each building type an ally may build next to. The stock construction yards, `GACNST` and `NACNST`, already have it. A building type without the flag no longer anchors an ally's placements.
targets:
- type: system
  id: base-adjacency
  effect: changed
- type: format
  id: spawn-ini
  effect: changed
- type: key
  id: EligibileForAllyBuilding
  effect: added
- type: key
  id: BaseNormal
  effect: changed
- type: key
  id: BuildOffAllyAnyStructure
  effect: removed
credit:
- ZivDero
- Iran
- AlexB
- CCHyper
- tomsons26
- MentalHomiega
---

`BuildOffAlly=Yes` in the `[Settings]` section of the client launch file, `SPAWN.INI`, lets the player place a structure next to an ally's building as well as their own, when that building's type sets `EligibileForAllyBuilding=yes` in its section of `rules.ini`. The stock rules set it on the two construction yards, so with them allies build next to each other's construction yards only.

Before, an ally's building needed `BaseNormal=yes` and a two-way alliance. By default, `BuildOffAllyAnyStructure=yes` under `[MultiplayerDefaults]` then let any such structure anchor. That key is removed. Now the ally's building anchors only when its owner counts the placing player as an ally, and the alliance does not have to run both ways. An ally's building needs `EligibileForAllyBuilding=yes` instead of `BaseNormal=yes`.

When the player places a wall, cells the ally owns, such as its walls and bibs, do not count. The setting does not change where computer houses build.

Iran is credited for the CnCNet spawner feature this follows, AlexB for the ts-patches patch that corrected it, and CCHyper and tomsons26 for the Vinifera version. MentalHomiega made the anchor test follow its owner's alliance and the per-type flag.
