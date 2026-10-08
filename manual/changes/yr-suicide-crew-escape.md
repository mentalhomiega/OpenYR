---
title: Let a vehicle that fires a suicide weapon leave a crew
category: fix
release: 0.2.0
targets:
- type: key
  id: Suicide
  effect: changed
credit: [MentalHomiega]
---

A vehicle that fires a `Suicide=yes` weapon now rolls for a crew under [`CrewEscape`](/keys/crewescape/), as it does for any other destruction. As in Yuri's Revenge, we no longer block that roll. Before, the firer left no crew.
