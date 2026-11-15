---
title: Let a structure type set how long its construction animation takes (BuildupTime)
category: feature
release: 0.2.0
targets:
- type: key
  id: BuildupTime
  scope: buildingtype
  effect: added
credit: [MentalHomiega]
---

A structure type with `BuildupTime` now plays its construction animation over that many game minutes instead of the `[General]` value, as Ares does, and never skips steps of it. Types that leave the key out are unchanged.
