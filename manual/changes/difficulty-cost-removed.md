---
title: Stop the Cost= multipliers from changing prices
category: fix
release: 0.2.0
targets:
- type: key
  id: Cost
  scope: difficulty-settings
  effect: changed
- type: key
  id: Cost
  scope: housetype
  effect: changed
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

Before, a house paid each object's `Cost=` times the difficulty `Cost=` and, outside a campaign, the country `Cost=`, so a computer house reading a 1.2 section paid 20 percent more. Yuri's Revenge reads neither, so we now ignore both. The country's `CostInfantryMult` family and the factory plant bonuses still apply.
