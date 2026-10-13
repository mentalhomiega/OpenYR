---
title: Climb a targetless missile before it detonates
category: fix
release: 0.2.0
targets:
- type: key
  id: MissileSafetyAltitude
  effect: added
- type: system
  id: projectile-flight
  effect: changed
credit:
- MentalHomiega
---

A homing projectile whose target is gone now climbs at 45 degrees and detonates once it is `MissileSafetyAltitude` above the ground, instead of at the corner of the map. A projectile slower than one lepton a frame detonates where it is. `MissileSafetyAltitude` is set in `[General]` of `rulesmd.ini`.
