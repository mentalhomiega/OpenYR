---
title: Read AffectsEnemies on warheads
category: feature
release: 0.2.0
targets:
- type: key
  id: AffectsEnemies
  effect: added
credit:
- MentalHomiega
---

`AffectsEnemies=no` in a warhead section of `rules.ini` makes the warhead do nothing to objects whose owner is not an ally of the attacker. It is the counterpart of `AffectsAllies`; the default is `yes`.
