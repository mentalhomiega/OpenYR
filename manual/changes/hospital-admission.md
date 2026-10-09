---
title: Keep an unlimited hospital's ammo and refuse controlling or mind-controlled infantry
category: fix
release: 0.2.0
targets:
- type: key
  id: Hospital
  effect: changed
- type: key
  id: Ammo
  effect: changed
credit:
- MentalHomiega
---

A hospital whose type sets no `Ammo` keeps its count of `-1` and takes any number of patients. Before this change the first admission dropped the count to `0`, so the hospital refused everyone after the first patient.

A hospital or armory also refuses infantry that holds a mind-controlled unit or is mind controlled itself, as gamemd's building receive handler does.
