---
title: Leave radiation where weapons go off
category: feature
release: 0.2.0
targets:
- type: system
  id: radiation
  effect: added
- type: key
  id: RadLevel
  effect: added
- type: key
  id: RadDurationMultiple
  effect: added
- type: key
  id: RadLevelDelay
  effect: added
- type: key
  id: RadApplicationDelay
  effect: added
- type: key
  id: RadLevelMax
  effect: added
- type: key
  id: RadLevelFactor
  effect: added
- type: key
  id: RadSiteWarhead
  effect: added
- type: key
  id: Radiation
  effect: added
- type: key
  id: ImmuneToRadiation
  effect: added
- type: key
  id: RadLightDelay
  effect: added
- type: key
  id: RadLightFactor
  effect: added
- type: key
  id: RadTintFactor
  effect: added
- type: key
  id: RadColor
  effect: changed
credit: [MentalHomiega]
---

A weapon with `RadLevel` now leaves radiation that damages vehicles, soldiers and landed aircraft standing in it, and glows in `RadColor` while it fades, as the Yuri's Revenge Desolator's does.
