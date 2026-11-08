---
title: Take a structure's laser look from its turret weapon
category: fix
release: 0.2.0
targets:
- type: key
  id: IsLaser
  effect: changed
credit:
- MentalHomiega
---

A structure that fires a laser draws the beam with the colors, house color setting and `LaserDuration` of its turret weapon, which is its primary weapon unless it has several turrets, whichever weapon fired. Before, the beam took them from the weapon that fired. A structure whose secondary weapon is a laser and whose primary weapon is not now shows the primary weapon's beam settings.
