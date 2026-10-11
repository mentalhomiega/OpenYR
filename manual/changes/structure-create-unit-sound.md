---
title: Structures can play a sound for each object they produce
category: feature
release: 0.2.0
targets:
- type: key
  id: CreateUnitSound
  effect: added
  scope: buildingtype
credit:
- MentalHomiega
---

`CreateUnitSound=` in a structure's `rulesmd.ini` section now plays a sound each time that structure finishes producing an object, as Yuri's Revenge does. The stock Yuri Grinder and Yuri Cloning Vats set it, but neither produces anything.
