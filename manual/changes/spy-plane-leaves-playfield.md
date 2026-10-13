---
title: Remove a spy plane once it leaves the playfield instead of hovering at the edge
category: fix
release: 0.2.0
targets:
- type: key
  id: FlyBy
  effect: changed
- type: key
  id: FlyBack
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A spy plane now holds its heading past its target and is removed once it leaves the playfield. Before this change it slowed to a stop at the edge of the local area and hovered there. A fly by or fly back aircraft is now removed at the playfield edge too; before, it was never removed.
