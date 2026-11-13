---
title: Read the Ares gather distances, Iron Curtain groups and Convert type script line
category: feature
release: 0.2.0
targets:
- type: mission
  id: TMISSION_CONVERT_TYPE
  effect: added
- type: mission
  id: TMISSION_AUXILIARY_POWER
  effect: added
- type: mission
  id: TMISSION_KILL_DRIVERS
  effect: added
- type: mission
  id: TMISSION_TAKE_VEHICLES
  effect: added
- type: mission
  id: TMISSION_GATHER_AT_ENEMY
  effect: changed
- type: mission
  id: TMISSION_GATHER_AT_BASE
  effect: changed
- type: mission
  id: TMISSION_IRON_CURTAIN_ME
  effect: changed
- type: key
  id: AIFriendlyDistance
  effect: added
- type: key
  id: Convert.Script
  effect: added
- type: key
  id: SW.Group
  effect: changed
credit: [MentalHomiega]
---

A script written for Ares now runs its team lines at Ares's numbers 65 to 68. Convert type (68) turns each vehicle of the team into the type that its own type names with `Convert.Script` in rulesmd.ini. Lines 65 to 67 are known but do nothing. The parameter of Gather at enemy base and Regroup at friendly base now adds to the gathering distance, and `AIFriendlyDistance` in the rulesmd.ini `[General]` section sets the distance of Regroup at friendly base. Iron Curtain me fires only the Iron Curtain whose `SW.Group` equals its parameter, which is `0` for a weapon that sets no group.
