---
title: Control the chronosphere with Chronoshift keys
category: feature
release: 0.2.0
targets:
- type: key
  id: Chronoshift.Allow
  effect: added
- type: key
  id: Chronoshift.Crushable
  effect: added
- type: key
  id: ChronoInfantryCrush
  effect: added
credit:
- MentalHomiega
---

`Chronoshift.Allow=no` in a type's rulesmd.ini section makes the chronosphere leave that object where it is. `Chronoshift.Crushable=no` makes a unit the chronosphere sets down on that object die instead of destroying it. `ChronoInfantryCrush=no` in `[General]` makes a chronoshifted infantryman die instead of destroying a vehicle on its landing cell. All three default to `yes`.
