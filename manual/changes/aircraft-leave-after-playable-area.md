---
title: Remove a loaner or team aircraft only after it has entered the playable area
category: fix
release: 0.2.0
targets:
- type: key
  id: FlyBy
  effect: changed
- type: key
  id: FlyBack
  effect: changed
credit:
- MentalHomiega
---

A loaner, or an aircraft of a team that is leaving the map, is removed by the leave-map rule only after it has been inside the playable area. Before, one that flew outside the local area without ever entering the playable area was removed at once. An aircraft on a paradrop approach or overfly is no longer removed by this rule.
