---
title: Draw demand-loaded animations with the generic art when theater art is missing
category: fix
release: 0.2.0
targets:
- type: key
  id: DemandLoad
  scope: animtype
  effect: changed
- type: key
  id: NewTheater
  scope: animtype
  effect: changed
- type: system
  id: building-animations
  effect: changed
credit:
- MentalHomiega
---

A demand-loaded animation whose theater file is missing now draws the generic art instead, the file with `G` as the second letter of its name, as Yuri's Revenge does. In URBAN, the structures `CALA10` and `CAMACH` now show their animations `CALA10_A` and `CAMACH_F`, which drew nothing before. A missing file is no longer reread from the archives on every frame.
