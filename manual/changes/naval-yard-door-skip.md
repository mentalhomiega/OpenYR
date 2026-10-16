---
title: Skip the door sequence when a naval yard releases a ship
category: fix
release: 0.2.0
targets:
- type: key
  id: Naval
  effect: changed
credit:
- MentalHomiega
---

A `Naval=yes` yard no longer runs its door sequence when a ship leaves: it does not open the door, clear the bib, wait for the ship to leave or close the door. It starts `ProductionAnim=` and goes idle on its next logic pass, as the naval branch of gamemd does.
