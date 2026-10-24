---
title: Release a parasite when a teleporter jumps
category: fix
release: 0.2.0
targets:
- type: system
  id: parasites
  effect: changed
- type: key
  id: Teleporter
  effect: changed
credit: [MentalHomiega]
---

A Terror Drone inside a `Teleporter=yes` unit, such as a Chrono Miner, is now released when the unit jumps back to its refinery. The drone is set down on the nearest cell it can enter, and the unit lands at the refinery without it. Before, the drone went along and stayed inside the unit.
