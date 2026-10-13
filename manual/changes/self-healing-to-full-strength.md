---
title: Let self-healing objects heal to full strength
category: fix
release: 0.2.0
targets:
- type: system
  id: repair
  effect: changed
- type: key
  id: SelfHealCap
  effect: changed
credit: [MentalHomiega]
---

An object with `SelfHealing=yes` or the `SELF_HEAL` veterancy ability now heals to full strength, as in Yuri's Revenge. It used to stop just past `ConditionYellow`, half strength at the defaults. `SelfHealCap` and `SelfHealingCap` still set a lower ceiling when given.
