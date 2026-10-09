---
title: Start the skirmish lobby's match options from the rules
category: fix
release: 0.2.0
targets:
- type: format
  id: multiplayer-rules
  effect: changed
- type: system
  id: skirmish-setup
  effect: changed
- type: key
  id: ShortGame
  effect: added
- type: key
  id: BuildOffAlly
  effect: added
- type: key
  id: MCVRedeploys
  effect: added
- type: key
  id: MultiEngineer
  effect: added
- type: key
  id: SuperWeaponsAllowed
  effect: added
- type: key
  id: FogOfWar
  scope: global-rules
  effect: added
- type: key
  id: GameSpeed
  scope: global-rules
  effect: added
credit: [MentalHomiega]
---

Until a match starts from the skirmish screen, its Fog Of War, Re-Deployable MCV, Multi Engineer, Superweapons, Build Off Ally, Short Game and Game Speed show the values of `[MultiplayerDialogSettings]`, or of `[MultiplayerDefaults]` where that section sets them. With the stock rules, Short Game, Build Off Ally, Re-Deployable MCV and Superweapons start on, and Game Speed starts at 5. Before, they started at fixed values: Short Game and Build Off Ally off, Re-Deployable MCV off, and Game Speed at 6.
