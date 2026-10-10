---
title: Stop the BuildTime= multipliers from changing build times
category: fix
release: 0.2.0
targets:
- type: key
  id: BuildTime
  scope: difficulty-settings
  effect: changed
- type: key
  id: BuildTime
  scope: housetype
  effect: changed
- type: key
  id: GameSpeedBias
  effect: changed
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

Before, a house built at its type's build time times the difficulty `BuildTime=`, the country `BuildTime=` outside a campaign, and `GameSpeedBias`, so a computer house reading a 1.2 section built 20 percent more slowly. Yuri's Revenge reads none of them, so we now ignore all three. The power, factory-count and wall factors still apply.
