---
title: Order sidebar cameos as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: changed
- type: key
  id: ConsideredAircraft
  effect: added
credit:
- MentalHomiega
---

Sidebar strips now use Yuri's Revenge's order: superweapons by recharge time, the player's own side's types, ground types before aircraft and ships, then technology level, cost and name. A mod that relies on [`CameoSortOrder`](/keys/cameosortorder/) sets [`UseCameoSortOrder=yes`](/keys/usecameosortorder/) to keep the previous order.
