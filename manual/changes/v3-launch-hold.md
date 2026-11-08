---
title: Hold a spawner's move while its missile leaves the pad
category: fix
release: 0.2.0
targets:
- type: key
  id: MissileLaunchHoldsMove
  effect: added
credit: [MentalHomiega]
---

A spawner such as the V3 Launcher no longer turns and moves while its missile is still on the pad. The missile now leaves along the facing the spawner had when it fired. `MissileLaunchHoldsMove=no` restores the earlier movement.
