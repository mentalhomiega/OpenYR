---
title: Play the destroyed-structure sound
category: fix
release: 0.2.0
targets:
- type: key
  id: BuildingDieSound
  effect: added
credit:
- MentalHomiega
---

Before, a structure that was destroyed made no sound, because we read the Tiberian Sun key `CrumbleSound` and the Yuri's Revenge rules name the sound `BuildingDieSound`. We now read `BuildingDieSound` as well, so a destroyed structure plays `BuildingGenericDie` as it does in Yuri's Revenge. A type with its own `DieSound` plays that instead. A `CrumbleSound` entry still works in a mod that has no `BuildingDieSound`.
