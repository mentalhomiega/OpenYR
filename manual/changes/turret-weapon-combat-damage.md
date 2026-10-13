---
title: Judge a turreted vehicle's combat damage by its turret weapon
category: fix
release: 0.2.0
targets:
- type: key
  id: TurretCount
  effect: changed
credit:
- MentalHomiega
---

A vehicle with [`TurretCount`](/keys/turretcount/) above `0` that is not a gattling type now takes its combat damage from the weapon its turret fires. Before, the average of both weapon slots set it. A gunner vehicle whose second slot is a healing weapon, such as an IFV's `RepairBullet`, then read as a healer and refused to fire at enemy objects, on the ground and in the air.
