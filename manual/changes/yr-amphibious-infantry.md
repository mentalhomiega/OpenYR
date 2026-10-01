---
title: Let amphibious infantry swim
category: feature
release: 0.2.0
targets:
- type: key
  id: SpeedType
  scope: aircrafttype
  effect: added
- type: key
  id: SpeedType
  scope: unittype
  effect: removed
- type: key
  id: EnterWaterSound
  effect: added
- type: key
  id: LeaveWaterSound
  effect: added
credit: [Lucas]
---

Infantry and aircraft now read `SpeedType`, so Tanya, the Navy SEAL and Yuri Prime walk into water as in Yuri's Revenge. In water they use their swimming sequences and play their water sounds, and they no longer die for standing there.
