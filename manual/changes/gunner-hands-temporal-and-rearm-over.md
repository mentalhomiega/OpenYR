---
title: Hand a gunner's temporal and rearm over when it boards or leaves
category: fix
release: 0.2.0
targets:
- type: system
  id: gunner-vehicles
  effect: changed
credit:
- MentalHomiega
---

A `Gunner=yes` vehicle takes over a temporal weapon from its first passenger and hands it back to the last passenger that unloads. A rearm still running on the side that gives up the temporal restarts on the receiving side with one more burst.
