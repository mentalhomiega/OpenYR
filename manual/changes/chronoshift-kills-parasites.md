---
title: Kill parasites in units a chronoshift touches
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
- type: system
  id: parasites
  effect: changed
credit:
- MentalHomiega
---

A chronosphere warp now kills a parasite inside any unit on the cells it covers. It also leaves alone a unit that is already being chronoshifted or is held by a temporal beam. Before, the parasite stayed inside, and such a unit was moved a second time.
