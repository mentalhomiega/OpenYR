---
title: Keep a naval yard's exit animation running
category: fix
release: 0.2.0
targets:
- type: key
  id: Naval
  effect: changed
- type: key
  id: ProductionAnimDamaged
  effect: changed
credit:
- MentalHomiega
---

A `Naval=yes` yard with `UnitRepair=yes` now keeps its `ProductionAnim=` playing while a ship leaves, so the animation runs to its end and the idle animation returns. Before, the yard ended the production animation on its next logic pass, so the idle animation, with the arch and crane, stayed off after the ship left. A weapons factory below `ConditionYellow` now plays `ProductionAnimDamaged=` when a unit leaves its door; it played the healthy animation before.
