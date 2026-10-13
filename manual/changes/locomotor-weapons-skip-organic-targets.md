---
title: Stop locomotor weapons firing at Organic targets
category: fix
release: 0.2.0
targets:
- type: key
  id: IsLocomotor
  effect: changed
- type: key
  id: Organic
  effect: changed
credit:
- MentalHomiega
---

A warhead with `IsLocomotor=yes` no longer fires at a target whose type sets `Organic=yes`, whatever the warhead's `Verses=` for the target's armor. Infantry types default to `Organic=yes`, so the Magnetron no longer fires at infantry whose armor its beam hits, such as `DNOB`, which has light armor. Targets that are not Organic are unchanged.
