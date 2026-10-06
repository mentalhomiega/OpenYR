---
title: Play superweapon buildings' charge animations
category: feature
release: 0.2.0
targets:
- type: key
  id: ChargedAnimTime
  effect: added
credit:
- MentalHomiega
---

A building that owns a superweapon now plays its `SuperAnim`, `SuperAnimTwo`, `SuperAnimThree` and `SuperAnimFour` animations as the weapon charges, becomes ready and is fired, switching at the time `ChargedAnimTime=` gives. Saves made by earlier builds no longer load, because building types now save the setting.
