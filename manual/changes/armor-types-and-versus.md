---
title: Declare armor types and set damage per armor
category: feature
release: 0.2.0
targets:
- type: system
  id: armor-types
  effect: added
credit:
- MentalHomiega
---

`[ArmorTypes]` in `rules.ini` or a map's rule overrides declares new armor types, which objects use through `Armor=`. A warhead sets its damage against any armor type, the original eleven included, with `Versus.<armor>=`, and can allow or forbid forced fire, retaliation and automatic targeting per armor with `Versus.<armor>.ForceFire=`, `.Retaliate=` and `.PassiveAcquire=`. The key names follow the Ares documentation.
