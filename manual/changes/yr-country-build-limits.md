---
title: Limit building by country and stolen technology
category: feature
release: 0.2.0
targets:
- type: key
  id: RequiredHouses
  effect: added
- type: key
  id: ForbiddenHouses
  effect: added
- type: key
  id: RequiresStolenAlliedTech
  effect: added
- type: key
  id: RequiresStolenSovietTech
  effect: added
- type: key
  id: RequiresStolenThirdTech
  effect: added
- type: system
  id: production
  effect: changed
credit: [Lucas]
---

`RequiredHouses` and `ForbiddenHouses` limit a type to some countries, and the three `RequiresStolen*Tech` keys make a type wait for stolen technology, as in Yuri's Revenge. Nothing steals technology yet, so types needing it cannot be built.
