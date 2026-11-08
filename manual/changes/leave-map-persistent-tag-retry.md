---
title: Stop a persistent tag from springing again forever when a team leaves
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_LEAVES_MAP
  effect: changed
credit: [MentalHomiega]
---

When a team that was leaving the map is dissolved, each logic tag is offered the leave-map event once. Before, a persistent tag whose ambient-light event still held, such as `TEVENT_AMBIENT_GREATER_THAN` in full daylight, sprang again on every offer, so the game stopped responding. A tag that removes itself from the list still passes the event on to the next tag.
