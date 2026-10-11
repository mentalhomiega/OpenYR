---
title: Let the computer raise its force shield, and run actions 140 and 141
category: feature
release: 0.2.0
targets:
- type: action
  id: TACTION_SET_DEFENSIVE_TARGET_CELL
  effect: changed
- type: action
  id: TACTION_CLEAR_DEFENSIVE_TARGET_CELL
  effect: changed
- type: key
  id: AIDefendAgainst
  effect: added
- type: key
  id: AISuperDefenseDistance
  effect: added
- type: key
  id: AISuperDefenseFrames
  effect: added
- type: key
  id: AISuperDefenseProbability
  effect: added
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A computer house now fires its charged force shield over a defensive cell, as Yuri's Revenge's does. A weapon whose type sets `AIDefendAgainst=yes` and that is fired within `AISuperDefenseDistance` of a computer base makes that house roll `AISuperDefenseProbability` and, on success, want its shield over its first construction yard for `AISuperDefenseFrames` frames. Actions 140 and 141 set and clear the cell, and did nothing before.

A house keeps its defensive cell in a save, and a super weapon type and the rules each read the new keys, so the save revision went up twice.
