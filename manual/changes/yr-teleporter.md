---
title: Drive teleporting harvesters and teleport them home
category: feature
release: 0.2.0
targets:
- type: key
  id: Teleporter
  effect: added
credit: [MentalHomiega]
---

`Teleporter=yes` vehicles now drive ordinary moves and use their own locomotor only to reach the refinery they dock at, as Yuri's Revenge's Chrono Miner does. The Teleport locomotor now turns its object when asked, so a teleporting harvester can back into the refinery. Before, the Chrono Miner never left the spot it was created on, and refineries earned nothing.
