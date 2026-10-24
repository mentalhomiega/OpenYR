---
title: Draw the cursors past frame 449 from the cursor sheet MOUSE.SHA
category: fix
release: 0.2.0
targets:
- type: key
  id: MigAttackCursor
  effect: changed
credit:
- MentalHomiega
---

The airstrike cursor shows over a structure that allows C4, as [`MigAttackCursor=yes`](/keys/migattackcursor/) describes. Before, the cursor drew nothing: its art is at frames 480 to 487, and the game loaded the 450-frame copy of the cursor sheet, `MOUSE.SHP`, from the base archive. The game now loads `MOUSE.SHA`, the 517-frame sheet that Yuri's Revenge uses.

The change also restores the cursors at frames 450 and up: Force Shield, Genetic Mutator, Psychic Dominator, Psychic Reveal and the spy plane. The frames from 0 to 449 are identical in both sheets, so the other cursors look the same.
