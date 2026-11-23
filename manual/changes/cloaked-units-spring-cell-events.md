---
title: Spring cell events for cloaked units
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_PLAYER_ENTERED
  effect: changed
- type: event
  id: TEVENT_ENTERS_ZONE
  effect: changed
- type: event
  id: TEVENT_CROSS_HORIZONTAL
  effect: changed
- type: event
  id: TEVENT_CROSS_VERTICAL
  effect: changed
- type: event
  id: TEVENT_SPY_ENTERING_AS_HOUSE
  effect: changed
- type: event
  id: TEVENT_SPY_ENTERING_AS_INFANTRY
  effect: changed
credit: [MentalHomiega]
---

A fully cloaked infantryman or vehicle now springs the cell events as any other one does when it finishes moving into a cell. Before, a cloaked object entered tagged cells, crossed lines and entered zones without satisfying the events, which Yuri's Revenge does not do.
