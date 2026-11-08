---
title: Pitch a vehicle's voxel barrel about the turret origin
category: fix
release: 0.2.0
targets: []
credit: [MentalHomiega]
---

A pitched barrel on a voxel-turret vehicle now pivots about the turret origin, as gamemd's voxel draw does. Before, it pivoted about the weapon's `FireFLH`, which moved a raised barrel off its turret-relative position. A barrel at level is drawn as before.
