---
title: Let a paradrop plane fly past an edge target and leave instead of hovering there
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A paradrop superweapon's plane aimed at a cell on the playfield edge now flies past it, makes its passes and leaves the map. Before, such a plane could stop at the edge and stay there. Its paratroopers drop only while the plane is inside the local view, so a target outside the view gets drops only when the plane passes through the view within the drop radius of the target.
