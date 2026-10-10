---
title: Weapons with TerrainFire=yes take the attack cursor over terrain
category: feature
release: 0.2.0
targets:
- type: key
  id: TerrainFire
  effect: added
- type: key
  id: TreeTargeting
  effect: changed
credit:
- MentalHomiega
---

A `TerrainFire=yes` weapon shows the attack cursor over a terrain object and over an `IsARock=yes` overlay cell, as gamemd does. Before, only `[CombatDamage] TreeTargeting=yes` put the cursor over a terrain object.
