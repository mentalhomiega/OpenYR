---
title: Skip flames whose animation is not set
category: fix
release: 0.2.0
targets:
- type: key
  id: SmallFire
  effect: changed
- type: key
  id: LargeFire
  effect: changed
credit: [MentalHomiega]
---

An unset `SmallFire` or `LargeFire` no longer crashes the game when a structure is damaged or destroyed, or when a flame or scorch animation lays fire. No flame appears instead. Yuri's Revenge's rules do not set `SmallFire`, so damaging a structure crashed every game.
