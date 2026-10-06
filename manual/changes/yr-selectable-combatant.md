---
title: Read IsSelectableCombatant for multiple selection
category: fix
release: 0.2.0
targets:
- type: key
  id: IsSelectableCombatant
  effect: added
- type: system
  id: band-selection
  effect: changed
credit:
- MentalHomiega
---

`IsSelectableCombatant=yes` in rulesmd.ini was ignored. A structure of such a type is now selected, along with the player's units, by the Select View command and by a selection box.
