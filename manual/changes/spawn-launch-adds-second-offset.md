---
title: Add the second spawn offset to the launch point
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
- type: key
  id: SecondSpawnOffset
  effect: changed
- type: system
  id: firing-geometry
  effect: changed
credit:
- MentalHomiega
---

A carried aircraft or missile now leaves from the firing point of its launch slot, and [`SecondSpawnOffset`](/keys/secondspawnoffset/#scope-aircrafttype) is added to that point instead of replacing it. The launch slot is the first slot when that slot's weapon has `Spawner=yes`, and the second slot otherwise. A missile launched by an object whose first weapon has a `Burst` above `1` takes the offset on the odd-numbered launches. Any other launch takes it only while the object's own burst index is not zero, which a burst weapon sets while it fires. A CMisl missile also leaves 40 leptons back on both the X and Y axes.

Before, the odd-numbered launches left from the object's center plus the offset, and the others from the first slot's firing point.
