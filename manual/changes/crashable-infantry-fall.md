---
title: Make a destroyed Crashable jumpjet soldier fall and crash
category: fix
release: 0.2.0
targets:
- type: key
  id: Crashable
  effect: changed
credit:
- MentalHomiega
---

A soldier with `Crashable=yes` and a jumpjet locomotor, such as the Rocketeer, no longer vanishes when he is destroyed in the air. The `InfantryExplode` animation plays where he dies, then he falls at his `JumpjetCrash` speed, turning as he drops, and plays his air death sequences. On landing he plays `AirDeathFinish` and is removed. A soldier destroyed on the ground dies as before.
