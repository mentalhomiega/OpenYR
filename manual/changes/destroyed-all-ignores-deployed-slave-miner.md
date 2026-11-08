---
title: Ignore a deployed Slave Miner in Destroyed all units and Destroyed all
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_UNITS_DESTROYED
  effect: changed
- type: event
  id: TEVENT_ALL_DESTROYED
  effect: changed
credit:
- MentalHomiega
---

The Destroyed all units and Destroyed all events no longer wait for a house's deployed Slave Miners, as in Yuri's Revenge. They read the units and soldiers the house has standing on the map, and a Slave Miner that has deployed is a structure there. Before, a Yuri house whose Slave Miners had deployed into Ore Refineries away from its base was never seen as destroyed, so Allied mission 4 did not end after the player had destroyed the Yuri base.
