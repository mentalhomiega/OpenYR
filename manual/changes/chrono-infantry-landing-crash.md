---
title: Stop the game crashing when a chronoshifted infantryman lands
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---
An infantryman that a chronosphere moves, such as a `Teleporter=yes` type, could crash the game when it landed. The landing handler used its own state after the unit's arrival replaced the unit's movement. The unit now lands, looks around and carries on.
