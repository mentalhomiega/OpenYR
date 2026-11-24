---
title: Block a spawner firing while its missile is launching
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit: [MentalHomiega]
---

A unit that spawns missiles or aircraft, such as the V3 Launcher, cannot fire again while a spawn is still leaving it. This covers a missile on its pad or taking off, and any spawn that is taking off. The unit can fire again once none is leaving.
