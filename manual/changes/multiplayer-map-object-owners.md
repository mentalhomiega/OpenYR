---
title: Leave out map objects owned by a playing country in multiplayer
category: fix
release: 0.2.0
targets:
- type: format
  id: scenario-objects
  effect: changed
credit:
- MentalHomiega
---

In a skirmish or multiplayer game, a map's units, infantry, aircraft and structures owned by a country that a player or a computer opponent plays are no longer created, as in Yuri's Revenge. They used to join that player's forces: on "Let There Be Fight" a player on the Americans side received about 30 civilians spread over the map, and the game opened with the view away from the player's start.
