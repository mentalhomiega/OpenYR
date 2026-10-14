---
title: Raise radar events where Yuri's Revenge raises them
category: fix
release: 0.2.0
targets:
- type: enum
  id: RadarEventType
  effect: changed
credit:
- MentalHomiega
---

Radar events now come from the places Yuri's Revenge raises them, for more of the game. Factory output, unit loss, repair, infiltration, owner change, building placement, superweapon launches and bridge repair raise their ping and line for the player. Beacons from other houses also raise an event when the owner and the player are allies. Each line plays only when its event is created, so a suppressed event stays silent.

A player's harvester that a temporal weapon starts to warp now raises its ping and the ore miner line. A gatherer building that undeploys, when attacked, raises the same ping and line instead of the base attack line. A destroyed harvester raises only the unit-lost event.

The space-bar jump also reaches two places that raise no radar event: a building destroyed by the player's own kill, and reinforcements from an allied house.
