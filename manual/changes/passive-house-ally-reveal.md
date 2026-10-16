---
title: Keep a passive house's objects from revealing ground in a campaign
category: fix
release: 0.2.0
targets:
- type: key
  id: MultiplayPassive
  effect: changed
- type: key
  id: AllyReveal
  effect: changed
credit: [ZivDero, Rampastring]
---

In a campaign, the objects of a country with `MultiplayPassive=yes` in its `rules.ini` section now make no [looks](/systems/map-visibility/#whose-looks-count), as they already did in other games. Before, with `AllyReveal=yes` in the `[AudioVisual]` section of `rules.ini`, an alliance with the player revealed the ground around each of those objects, and the regrowth passes kept the ground around its allied structures uncovered. Now an alliance reveals nothing from them, and the regrowth passes do not keep that ground uncovered.
