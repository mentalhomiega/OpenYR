---
title: Give SubjectToElevation weapons range from higher ground
category: fix
release: 0.2.0
targets:
- type: key
  id: SubjectToElevation
  effect: added
- type: key
  id: ElevationIncrement
  effect: added
- type: key
  id: ElevationIncrementBonus
  effect: added
- type: key
  id: ElevationBonusCap
  effect: added
credit: [MentalHomiega]
---

`SubjectToElevation` and the `[ElevationModel]` keys in rulesmd.ini were ignored. A direct-fire weapon whose projectile sets `SubjectToElevation=yes` now reaches farther from higher ground, as in Yuri's Revenge.
