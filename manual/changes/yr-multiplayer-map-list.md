---
title: List the Yuri's Revenge multiplayer maps by name
category: fix
release: 0.2.0
targets:
- type: key
  id: Description
  scope: map-packets
  effect: changed
credit:
- MentalHomiega
---

Skirmish setup and the multiplayer map list now offer the maps that `MISSIONSMD.PKT` lists, under the text of the string table label each map's `Description=` names, such as "The Alamo (2)". The list used to come from `MISSIONS.PKT`, a Tiberian Sun list of maps the game does not ship, so skirmish setup showed "Unable to read scenario!" and then opened with no map and no preview.

Skirmish setup now opens on the first listed map whose file is present. In a network game, a comma in the map's name reaches the other players as a semicolon, so a name such as "Anytown, Amerika (2-4)" no longer stops them from finding the map.
