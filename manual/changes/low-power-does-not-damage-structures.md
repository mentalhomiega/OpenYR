---
title: Stop damaging structures while a house is short of power
category: fix
release: 0.2.0
targets:
- type: system
  id: power
  effect: changed
- type: key
  id: DamageDelay
  effect: changed
credit:
- MentalHomiega
---

A house short of power no longer wears down its structures, as in Yuri's Revenge. Before, every structure that consumed power lost 1 point of strength each `DamageDelay` minutes, down to `ConditionYellow`. `DamageDelay` is still read and has no effect.
