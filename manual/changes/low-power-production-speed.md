---
title: Slow production in proportion to the power shortfall
category: fix
release: 0.2.0
breaking: true
migration:
- Replace `MinProductionSpeed=` under `[General]` with `MinLowPowerProductionSpeed=`, which takes its place; the old key has no effect.
targets:
- type: key
  id: MinProductionSpeed
  effect: removed
- type: key
  id: MinLowPowerProductionSpeed
  effect: added
- type: key
  id: MaxLowPowerProductionSpeed
  effect: added
- type: key
  id: LowPowerPenaltyModifier
  effect: added
- type: system
  id: power
  effect: changed
credit: [MentalHomiega]
---

We now slow production in proportion to a house's power shortfall. A house at 80 percent power builds at speed 0.8, so a build takes a quarter longer. Before, the speed was a fixed step, so the same house built a third longer.

`MinProductionSpeed` in `[General]` is no longer read. `MinLowPowerProductionSpeed` takes its place in the same section, with the same default.

`MaxLowPowerProductionSpeed` caps the speed of a house short of power, and `LowPowerPenaltyModifier` sets how strongly a shortfall slows production. The stock rules set the cap to `.8`, so a house short of power builds at no more than 0.8 speed.
