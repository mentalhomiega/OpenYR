---
title: Give each aircraft type its own reload interval (ReloadRate)
category: feature
release: 0.2.0
targets:
- type: key
  id: ReloadRate
  effect: added
  scope: aircrafttype
credit:
- MentalHomiega
---

An aircraft type now reads `ReloadRate=` as Ares does: a docked aircraft of that type gets its rounds from a rearming pad on its own timer at that interval. Aircraft types that leave the key out reload on the [General] interval as before.
