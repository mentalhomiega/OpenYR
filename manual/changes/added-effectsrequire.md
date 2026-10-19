---
title: Condition warhead effects on damage and Verses
category: feature
release: 0.2.0
targets:
- type: key
  id: EffectsRequireDamage
  effect: added
- type: key
  id: EffectsRequireVerses
  effect: added
credit:
- MentalHomiega
---

`EffectsRequireVerses=no` in a warhead section lets the driver kill of `KillDriver=yes` and the effect of `AttachEffect.Duration` reach an armor whose `Verses` is `0%`. `EffectsRequireDamage=yes` limits them to objects the hit damages by at least one point. The defaults, `yes` and `no`, keep the earlier behavior.
