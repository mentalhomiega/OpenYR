---
title: Set off death weapons and impact sounds when aircraft crash
category: feature
release: 0.2.0
targets:
- type: key
  id: Explodes
  scope: aircrafttype
  effect: changed
- type: key
  id: ImpactLandSound
  effect: added
- type: key
  id: ImpactWaterSound
  effect: added
- type: key
  id: C4Warhead
  effect: changed
credit: [MentalHomiega]
---

A destroyed aircraft that crashes now sets off its death weapon and plays its `ImpactLandSound` or `ImpactWaterSound`, as in Yuri's Revenge, instead of a `C4Warhead` blast.
