---
title: Play a map's houses by name and country
category: fix
release: 0.2.0
targets:
- type: key
  id: Country
  effect: added
- type: key
  id: ParentCountry
  effect: added
- type: key
  id: Player
  scope: scenarios
  effect: changed
- type: key
  id: Allies
  effect: changed
- type: key
  id: Suffix
  scope: housetype
  effect: changed
credit: [MentalHomiega]
---

A campaign map's `[Houses]` are now separate from its countries, as in Yuri's Revenge. Each house takes the rules or `[Countries]` country that its record names with `Country=`, the player is the house `[Basic] Player=` names, and objects, teams, triggers and `Allies=` name houses. Missions such as `SOV03UMD` and `ALL05UMD`, whose house names are not rules countries, load and give the player their own units. They used to crash.

A country of the map's `[Countries]` starts from the settings of its `ParentCountry=`. `Suffix=` keeps up to 31 characters, where a longer value used to be cut to three. A `Player=` that names no house now plays the first house with `PlayerControl=yes`. Houses of a Tiberian Sun map, which have no `Country=`, load as before.

Saved games made by earlier builds are refused, because a country's suffix is stored in full.
