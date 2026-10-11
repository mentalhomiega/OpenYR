---
title: Count garrisoned soldiers in a structure's threat
category: fix
release: 0.2.0
targets:
- type: key
  id: ThreatPerOccupant
  effect: added
- type: system
  id: base-attacked
  effect: changed
credit:
- MentalHomiega
---

A garrisoned structure now adds `ThreatPerOccupant` from `[General]` of `rulesmd.ini` for each soldier inside to the computer's threat map, in place of its `ThreatPosed`, as Yuri's Revenge does. The map follows soldiers moving in and out.
