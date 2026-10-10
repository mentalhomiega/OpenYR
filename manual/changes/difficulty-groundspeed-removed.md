---
title: Stop the ground speed multipliers from changing speed
category: fix
release: 0.2.0
targets:
- type: key
  id: Groundspeed
  scope: difficulty-settings
  effect: changed
- type: key
  id: Groundspeed
  scope: housetype
  effect: changed
- type: key
  id: GameSpeedBias
  effect: changed
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

Before, a house's vehicles and soldiers moved at their type's speed times the difficulty `Groundspeed=`, the country `Groundspeed=` outside a campaign, and `GameSpeedBias`, so a computer house reading a 1.2 section moved 20 percent faster. Yuri's Revenge reads none of them, so we now ignore all three and every object moves at the speed its type and the usual modifiers give it.
