---
title: Keep a garrison's firing turn when an occupant leaves the list
category: fix
release: 0.2.0
targets:
- type: system
  id: garrisons
  effect: changed
credit:
- MentalHomiega
---

A garrison's firing turn is no longer moved to the first occupant when an occupant is removed from its list. Before, a turn past the end of the shorter list was reset to the first occupant. Now the next shot fires the structure's own weapon, and the turn moves on from there. A garrison whose turn points at an occupant fires as before.
