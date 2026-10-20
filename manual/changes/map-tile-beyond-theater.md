---
title: Draw clear ground for a map tile the theater does not define
category: fix
release: 0.2.0
targets:
- type: format
  id: scenario-terrain
  effect: changed
credit:
- MentalHomiega
---

A map cell whose tile type is beyond the theater's tile types now draws clear ground. Before, such a cell could crash the game while the radar image was built at load.
