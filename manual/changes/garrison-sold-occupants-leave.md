---
title: Let a garrison's soldiers out when it is sold
category: fix
release: 0.2.0
targets:
- type: system
  id: garrisons
  effect: changed
credit: [MentalHomiega]
---

Selling a structure with soldiers inside now lets them out, as gamemd's selling mission does. We place each one on the nearest free cell, or on the structure's centre when no cell is free. Before, a sale did not let the soldiers out.
