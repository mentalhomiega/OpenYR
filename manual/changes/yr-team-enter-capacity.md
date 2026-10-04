---
title: Spread team members over bunkers and bio reactors with room
category: fix
release: 0.2.0
targets:
- type: mission
  id: TMISSION_ENTER_TANK_BUNKER
  effect: changed
- type: mission
  id: TMISSION_ENTER_BIO_REACTOR
  effect: changed
- type: mission
  id: TMISSION_ENTER_BATTLE_BUNKER
  effect: changed
credit: [MentalHomiega]
---

The enter tank bunker, enter bio reactor and enter battle bunker script lines now count the members already sent to a structure on the same line. A member goes to the nearest structure that still has room for it. Before, every member chose the same nearest structure, even one with room for only one of them.
