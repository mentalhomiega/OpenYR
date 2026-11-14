---
title: Make crushing an object hurt the crusher (CrushDamage)
category: feature
release: 0.2.0
targets:
- type: key
  id: CrushDamage
  effect: added
- type: key
  id: CrushDamage.Rookie
  effect: added
- type: key
  id: CrushDamage.Veteran
  effect: added
- type: key
  id: CrushDamage.Elite
  effect: added
- type: key
  id: CrushDamage.Warhead
  effect: added
credit: [MentalHomiega]
---

A vehicle that crushes an object whose type sets `CrushDamage` now takes that damage, by the crushed object's rank and through `CrushDamage.Warhead`, as Ares does. Types that leave the keys out are unchanged.
