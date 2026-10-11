---
title: Light the map for a nuclear flash and a dominator blast
category: feature
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
- type: key
  id: DominatorAmbient
  effect: added
- type: key
  id: DominatorAmbientChangeRate
  effect: added
- type: key
  id: DominatorBlue
  effect: added
- type: key
  id: DominatorGreen
  effect: added
- type: key
  id: DominatorGround
  effect: added
- type: key
  id: DominatorLevel
  effect: added
- type: key
  id: DominatorRed
  effect: added
- type: key
  id: NukeAmbientChangeRate
  effect: added
credit:
- MentalHomiega
---

The map now flashes when a nuclear blast starts, and takes a red tint while a psychic dominator blast lasts, as in Yuri's Revenge. A scenario's `[Lighting]` section sets the dominator's light and how fast either effect changes it. The dominator also stays busy until the light has settled back. Saves made by earlier builds no longer load, because a save now holds the flash and the dominator's lighting.
