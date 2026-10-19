---
title: Read a side's crew, technician, engineer, divisor and disguise
category: feature
release: 0.2.0
targets:
- type: key
  id: Crew
  effect: added
- type: key
  id: Engineer
  scope: side
  effect: added
- type: key
  id: Technician
  scope: side
  effect: added
- type: key
  id: DefaultDisguise
  effect: added
- type: key
  id: SurvivorDivisor
  effect: added
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

A side's section can set the infantry its crewed objects leave as survivors (`Crew=`, `Technician=` and `Engineer=`), the credits of a refund that make one survivor (`SurvivorDivisor=`) and the disguise of a `PermaDisguise` object (`DefaultDisguise=`). A side that sets none takes the `[General]` values for its place in `[Sides]` as before, so a mod can give a fourth side survivors, which the `[General]` keys cannot. The key names follow Ares.
