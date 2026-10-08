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

The computer's nuke now aims at the cell the ion cannon rating picks, as the lightning storm does. Before, it aimed at the enemy structure rated highest on the threat map, which included structures in limbo. A structure in limbo at cell 1,0 drew the nuke onto the map corner. A nuke that a trigger aims also waits for an enemy now.
