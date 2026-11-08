---
title: Add DamSmkOffScrnRel for screen-relative damage smoke offsets
category: feature
release: 0.2.0
targets:
- type: key
  id: DamSmkOffScrnRel
  effect: added
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

`DamSmkOffScrnRel=yes` makes a type's [`DamageSmokeOffset`](/keys/damagesmokeoffset/) a screen offset for its spark and smoke systems. The default is `no`, which keeps the offset as a world offset, so stock rules play as before. Save games now store the flag with the type, so a save from an earlier build is refused.
