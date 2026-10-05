---
title: Burn damage fires on badly damaged structures
category: fix
release: 0.2.0
targets:
- type: system
  id: structure-damage-fires
  effect: added
- type: key
  id: DamageFireTypes
  effect: added
credit:
- MentalHomiega
---

Structures burn when badly damaged, as in Yuri's Revenge; before, they showed no damage fires. `DamageFireTypes=` in `[General]` of rulesmd.ini lists the fire animations, and `DamageFireOffset0` to `DamageFireOffset7` in a structure's art section set where they burn. The fires start when the structure's health falls to `ConditionYellow` (`ConditionRed` for a structure that can be garrisoned) and go out when it recovers or is destroyed.
