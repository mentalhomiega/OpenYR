---
title: Time veinhole growth by VeinholeGrowthRate alone
category: fix
release: 0.2.0
targets:
- type: key
  id: VeinholeGrowthRate
  effect: changed
- type: key
  id: VeinholeShrinkRate
  effect: changed
credit: [MentalHomiega]
---

A veinhole now spreads its veins every `VeinholeGrowthRate` frames, and withers them every `VeinholeShrinkRate` frames, as in Yuri's Revenge. A random delay of up to half again that long was added to each step, so veins grew more slowly.
