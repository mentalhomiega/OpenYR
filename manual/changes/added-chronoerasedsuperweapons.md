---
title: Suspend superweapons of a chrono-erased structure
category: fix
release: 0.2.0
targets:
- type: key
  id: ChronoErasedSuperWeapons
  effect: added
credit:
- MentalHomiega
---

A structure that a Chrono Legionnaire is erasing no longer keeps its superweapon usable. A Chronosphere mid-erase could still teleport units before. The weapon is now suspended until the beam lets go of the structure or the structure is removed. `ChronoErasedSuperWeapons=yes` restores the old behavior. See [`ChronoErasedSuperWeapons`](/keys/chronoerasedsuperweapons/) for the exact rules.
