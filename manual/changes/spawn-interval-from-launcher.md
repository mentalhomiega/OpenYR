---
title: Space spawn launches by the launcher's MissileSpawn flag
category: fix
release: 0.2.0
targets:
- type: key
  id: MissileSpawn
  effect: changed
- type: system
  id: spawned-aircraft
  effect: changed
credit:
- MentalHomiega
---

A Dreadnought's two missiles now leave 20 frames apart, as in Yuri's Revenge. The gap is 9 frames only when the launching object's own type has `MissileSpawn=yes`. Previously the gap came from the spawned type's flag, so Dreadnought missiles left 9 frames apart.
