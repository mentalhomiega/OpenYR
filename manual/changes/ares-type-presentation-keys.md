---
title: Set parachutes, slam sounds, aircraft smoke, air damage, rings, water images, factory owners and cloning per type
category: feature
release: 0.2.0
targets:
- type: key
  id: Parachute.Anim
  effect: added
- type: key
  id: SlamSound
  effect: added
- type: key
  id: Smoke.Anim
  effect: added
- type: key
  id: Smoke.ChanceRed
  effect: added
- type: key
  id: Smoke.ChanceDead
  effect: added
- type: key
  id: DamageAirThreshold
  effect: added
- type: key
  id: RadialIndicatorRadius
  effect: added
- type: key
  id: WaterImage
  effect: added
- type: key
  id: FactoryOwners
  effect: added
- type: key
  id: FactoryOwners.Forbidden
  effect: added
- type: key
  id: Cloneable
  effect: added
credit:
- MentalHomiega
---

An object type can name its own parachute (`Parachute.Anim`), a structure its own placement sound (`SlamSound`), and an aircraft the smoke it leaves (`Smoke.Anim`, `Smoke.ChanceRed`, `Smoke.ChanceDead`). A warhead can damage objects in the air from the ground (`DamageAirThreshold`). A selected object can draw a ring of its own radius (`RadialIndicatorRadius`), a vehicle can be drawn as another type on water (`WaterImage`), a type can need factories first owned by certain countries (`FactoryOwners`, `FactoryOwners.Forbidden`), and a soldier can refuse cloning (`Cloneable`). The keys follow the Ares documentation, and a type that sets none of them behaves as before.
