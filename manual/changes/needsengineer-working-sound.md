---
title: Play a NeedsEngineer structure's working sound only after its capture
category: fix
release: 0.2.0
targets:
- type: key
  id: NeedsEngineer
  effect: changed
- type: key
  id: WorkingSound
  effect: changed
credit:
- MentalHomiega
---

A neutral tech oil derrick no longer plays its `WorkingSound` when the map places it. A `NeedsEngineer=yes` structure plays the sound when an engineer captures it, as gamemd does. Before this change the sound played at placement, and a capture did not play it again.
