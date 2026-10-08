---
title: Read the crate powerup fields in the game's order
category: fix
release: 0.2.0
targets:
- type: system
  id: crates
  effect: changed
credit: [MentalHomiega]
---

We now read each `[Powerups]` entry as share, animation, naval flag and number, as the game does. Before, we read the naval flag as the number, so `Armor=10,ARMOR,yes,1.5` gave the armor result a number of `0`, and the firepower and speed results likewise. The money result then paid between `0` and `CrateMoneyBonus` credits instead of `2000` to `2900`. In a skirmish or multiplayer game, a crate collected on water now gives money unless its result's naval flag is `yes`.
