---
title: Stop sparks on damaged vehicles, aircraft and structures
category: fix
release: 0.2.0
targets:
- type: key
  id: DamageParticleSystems
  effect: changed
credit:
- MentalHomiega
---

A damaged vehicle, aircraft or structure no longer throws sparks. In gamemd only Cyborg soldiers do, and the stock rules list `SparkSys` in the `DamageParticleSystems=` of nearly every vehicle for that reason alone. Before, a vehicle below yellow health started a `SparkSys` at random, which gamemd never does. The smoke that starts when a hit takes the object below yellow is unchanged.
