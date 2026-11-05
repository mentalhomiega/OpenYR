---
title: Random maps read their lighting, tiberium and trees from RMGMD.INI
category: feature
release: 0.2.0
targets:
- type: key
  id: RMGMinimumTiberium
  effect: added
- type: key
  id: RMGMaximumTiberium
  effect: added
- type: key
  id: RMGLevelLightSettings
  effect: added
- type: key
  id: TemperateAmbientLight
  effect: added
- type: key
  id: TemperateAmbientRed
  effect: added
- type: key
  id: TemperateAmbientGreen
  effect: added
- type: key
  id: TemperateAmbientBlue
  effect: added
- type: key
  id: SnowAmbientLight
  effect: added
- type: key
  id: SnowAmbientRed
  effect: added
- type: key
  id: SnowAmbientGreen
  effect: added
- type: key
  id: SnowAmbientBlue
  effect: added
- type: key
  id: RMGVegetationMinimums
  effect: added
- type: key
  id: RMGVegetationMaximums
  effect: added
- type: key
  id: MaxTrees
  effect: added
credit: [MentalHomiega]
---

A generated map now takes its ambient light, tints and height level light from the time-of-day lists of [RMGMD.INI](/formats/rmgmd-ini/), as in Yuri's Revenge. The file also sets how much tiberium each player adds, how many trees the woods may plant, and the range the map generator dialog's randomize button picks vegetation from. Snow maps are no longer dimmed to three quarters of the hour's light, so they no longer brighten when an ion storm ends.
