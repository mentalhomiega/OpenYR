---
title: Drop voxel vertical projectiles straight down
category: fix
release: 0.2.0
targets:
- type: key
  id: Vertical
  effect: changed
credit: [MentalHomiega]
---

Vertical voxel projectiles, such as the Kirov's bombs, now launch straight down, so they fall onto the target below and explode on the ground. Before, they launched straight up and exploded at DetonationAltitude, far from the target, so they did no damage. A voxel projectile that is not vertical now launches level instead of straight up.
