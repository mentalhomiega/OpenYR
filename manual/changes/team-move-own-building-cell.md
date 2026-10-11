---
title: Send a team to the cell nearest its own building that it can reach
category: fix
release: 0.2.0
targets:
- type: mission
  id: TMISSION_MOVE_TO_OWN_BUILDING
  effect: changed
credit:
- MentalHomiega
---

The Move to own building line now picks the cell nearest the structure that is clear for one cell and in the movement zone of the member's own cell. Before this it needed a clear 3 by 3 block and took the movement zone of the structure's cell.
