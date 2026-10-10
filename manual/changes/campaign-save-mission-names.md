---
title: Name campaign saves after the mission list
category: fix
release: 0.2.0
targets:
- type: format
  id: missionmd-ini
  effect: changed
credit:
- MentalHomiega
---

A campaign save is now described by the mission's name from `UIName=` in `MISSIONMD.INI`, as in Yuri's Revenge. Before, the description came only from a `Name=` entry in `MISSION.INI` or `MISSION<n>.INI`, which the mission list of Yuri's Revenge does not use, so these saves had no mission name.
