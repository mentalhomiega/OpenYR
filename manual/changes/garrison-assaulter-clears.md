---
title: Let an Assaulter soldier clear a garrison
category: feature
release: 0.2.0
targets:
- type: key
  id: Assaulter
  effect: added
- type: key
  id: AssaultAnim
  effect: added
- type: system
  id: garrisons
  effect: changed
credit: [MentalHomiega]
---

A soldier with `Assaulter=yes` and no `Occupier=yes` now moves into a structure that holds soldiers of a house it is not allied with. It kills every occupant, plays its primary weapon's `AssaultAnim` at each one, and steps aside, as gamemd's BuildingClass::KillOccupants does. No stock soldier sets `Assaulter=yes`, so the stock game plays the same.
