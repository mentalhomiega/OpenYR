---
title: Stop projectiles with solid buildings
category: feature
release: 0.2.0
targets:
- type: key
  id: SubjectToBuildings
  effect: added
- type: key
  id: SolidHeight
  effect: added
- type: key
  id: AlliedSolidTransparency
  effect: added
credit:
- MentalHomiega
---

A projectile with `SubjectToBuildings=yes` explodes on a building whose `SolidHeight=` is not `0` when it enters the building's cell below that height. `AlliedSolidTransparency=yes` in `[CombatDamage]` lets it through the buildings of its firer's allies.
