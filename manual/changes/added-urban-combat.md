---
title: Pass shots at an occupied structure to its occupants
category: feature
release: 0.2.0
targets:
- type: key
  id: UC.PassThrough
  effect: added
- type: key
  id: UC.FatalRate
  effect: added
- type: key
  id: UC.DamageMultiplier
  effect: added
- type: key
  id: SubjectToTrenches
  effect: added
credit:
- MentalHomiega
---

A building section in `rules.ini` with `UC.PassThrough=` above `0%` and at least one occupant lets that share of the shots at it hit an occupant instead of the building. `UC.FatalRate=` is the share of those that kill the occupant outright, and `UC.DamageMultiplier=` scales the damage of the rest. A projectile with `SubjectToTrenches=no` passes all its shots through.
