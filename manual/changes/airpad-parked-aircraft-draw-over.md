---
title: Draw aircraft parked on an airpad over its landing markings
category: fix
release: 0.2.0
targets:
- type: key
  id: YSortAdjust
  effect: changed
credit: [MentalHomiega]
---

Objects on the ground layer are now ordered by their X plus Y coordinates, which is their depth on the isometric map, as in Yuri's Revenge. Before, only the Y coordinate was used. Some aircraft parked on an Allied Airforce Command's pad are now drawn over the pad's flat landing markings, which covered them before. Objects that do not overlap on screen look the same as before.
