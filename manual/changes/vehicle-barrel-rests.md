---
title: Keep a vehicle's barrel at its resting elevation
category: fix
release: 0.2.0
targets:
- type: key
  id: FireAngle
  effect: changed
credit:
- MentalHomiega
---

A vehicle's barrel no longer aims at its target. It stays at its [`FireAngle`](/keys/fireangle/) elevation, as gamemd's barrel does. Before, the barrel rose toward the target, to 15 to 30 degrees for a target 4 to 6.5 cells away, and a target beyond about 6 cells made the solve fail and the barrel fall back to a fixed angle. Aircraft, infantry and structures aim as before.
