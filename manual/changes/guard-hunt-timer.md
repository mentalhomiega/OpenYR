---
title: Make guard and hunt scans wait for the targeting timer
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
- type: key
  id: NormalTargetingDelay
  effect: changed
- type: key
  id: CanPassiveAquire
  effect: changed
credit:
- MentalHomiega
---

An object on a guard, move or harvest mission now takes its range scan for a target only when its targeting timer has run out, and only if it can opportunity-fire. Before, a guard object searched each time its mission ran. Each such scan restarts the timer with `NormalTargetingDelay` plus a random 0 to 2 frames, and a hunt scan restarts it the same way. A guarding building takes the target that this scan sets, and no longer searches itself.

An object takes no target on its own when its type sets `CanPassiveAquire=no`, or when it is unarmed, a slave, warping a target out, an occupiable structure with no occupant, a human-owned engineer, or out of capture control. Area guard uses the same test. Saves made before this change do not load.
