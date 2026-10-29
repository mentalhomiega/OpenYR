---
title: Start smoke when a structure is destroyed
category: feature
release: 0.2.0
targets:
- type: key
  id: DestroyParticleSystems
  effect: added
- type: key
  id: DestroySmokeOffset
  effect: added
credit:
- MentalHomiega
---

A structure type can now list smoke systems in `DestroyParticleSystems=`, as in Yuri's Revenge. One of them starts at `DestroySmokeOffset=` from the structure when it is destroyed. The stock rules use neither key.
