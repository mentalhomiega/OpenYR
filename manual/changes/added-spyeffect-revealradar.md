---
title: Let a spied radar reveal the owner's units (SpyEffect.RevealRadar)
category: feature
release: 0.2.0
targets:
- type: key
  id: SpyEffect.RevealRadar
  effect: added
credit: [MentalHomiega]
---

A spy that enters a `Radar=yes` structure with `SpyEffect.Custom=yes` and `SpyEffect.RevealRadar=yes` now makes the owner's units reveal the map to the spy's side until the structure is destroyed, sold or changes owner, as Ares does. Structures without the key are unchanged.
