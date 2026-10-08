---
title: Hold the computer's mutator and dominator while a trigger aims
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
- type: action
  id: TACTION_SET_PREFERRED_TARGET_CELL
  effect: changed
credit:
- MentalHomiega
---

A computer house with a Set Preferred Target Cell aim no longer fires its genetic mutator or psychic dominator on its own. Before, it picked a target for both, ignoring the aim. The aim still fires the paradrop, the spy plane and the psychic reveal, and the nuclear missile and the lightning storm while the house has an enemy.
