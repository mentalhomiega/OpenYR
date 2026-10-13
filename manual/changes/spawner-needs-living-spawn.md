---
title: Hold a Spawner shot while every carried craft is destroyed
category: fix
release: 0.2.0
targets:
- type: key
  id: Spawner
  effect: changed
credit:
- MentalHomiega
---

A [`Spawner=yes`](/keys/spawner/#scope-weapontype) weapon no longer sets a target for its carried craft while none of them is alive. The firer reports a rearm instead, as in Yuri's Revenge. A destroyed craft does not count as alive until it is replaced.
