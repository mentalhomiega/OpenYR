---
title: Add Yuri's Revenge's infantry death types
category: feature
release: 0.2.0
targets:
- type: key
  id: InfDeath
  effect: changed
- type: key
  id: InfantryHeadPop
  effect: added
- type: key
  id: InfantryNuked
  effect: added
- type: key
  id: InfantryVirus
  effect: added
- type: key
  id: InfantryMutate
  effect: added
- type: key
  id: InfantryBrute
  effect: added
credit: [Lucas]
---

`InfDeath` now takes Yuri's Revenge's values 6 to 10, with their animations named in `[AudioVisual]`. Dogs die like other soldiers, the burning figure no longer takes the local player's colors, and a death whose animation is unset removes the soldier instead of crashing the game. Mutation into a brute (`InfDeath=9`) is not supported yet.
