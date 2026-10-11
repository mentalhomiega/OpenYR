---
title: Detonate a targetless missile where it is
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

A homing projectile whose target is gone now detonates where it is, as Yuri's Revenge does, instead of at the corner of the map. A slow one detonates once it is `MissileSafetyAltitude` above the ground, set in `[General]` of `rulesmd.ini`.
