---
title: Step a gattling structure's barrel through its frames as it spins
category: fix
release: 0.2.0
targets:
- type: system
  id: gattling-weapons
  effect: changed
- type: key
  id: TurretAnimIsVoxel
  effect: changed
credit:
- MentalHomiega
---

A structure whose only voxel model is its gattling barrel, such as the Yuri Gattling Cannon, drew that barrel on its first frame, so the barrels never moved while the gun spun up and fired. The barrel now steps through its frames with the spin, as in Yuri's Revenge. A barrel under a separate voxel turret model now stays on its first frame, as in Yuri's Revenge; before, it stepped with the spin.
