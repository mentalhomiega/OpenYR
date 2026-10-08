---
title: Fire the spy entering trigger events
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_SPY_ENTERING_AS_HOUSE
  effect: changed
- type: event
  id: TEVENT_SPY_ENTERING_AS_INFANTRY
  effect: changed
credit: [MentalHomiega]
---

Spy entering as House... and Spy entering as Infantry... now fire. We offer each to the tag on a cell when a disguised soldier enters that cell, and each names the house or infantry type the soldier is disguised as. Before, neither event could fire. Both events now attach to a cell, not to a structure, as they do in gamemd.
