---
title: Aim the computer's nuke by the ion cannon rating
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

We now aim the computer's nuke at the cell the ion cannon rating picks, as the lightning storm does. Before, we aimed it at the enemy structure rated highest on the threat map, which included structures in limbo. A structure in limbo at cell 1,0 drew the nuke onto the map corner. We also wait for an enemy before a trigger-aimed nuke fires.
