---
title: Scan for idle targets at a turreted vehicle's current weapon range
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
credit:
- MentalHomiega
---

A vehicle with a turret, such as the IFV, now scans for idle targets out to the range of the weapon it is currently firing, not the longer of its first two weapons. A gunner vehicle carrying a sniper scans to the sniper's range. An empty gunner vehicle scans to its first weapon's range.
