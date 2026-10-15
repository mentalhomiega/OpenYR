---
title: Keep a spinning turret turning while it has a target
category: fix
release: 0.2.0
targets:
- type: key
  id: TurretSpins
  effect: changed
credit: [MentalHomiega]
---

A turret with `TurretSpins=yes` keeps turning while its unit holds a target, as the Floating Disc's does during a force-attack on the ground or on an object. Before, the turret turned to the target's bearing and stopped there, so the spin stopped for as long as the unit had a target.
