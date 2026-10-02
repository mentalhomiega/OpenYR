---
title: Fight from Tank Bunkers
category: feature
release: 0.2.0
targets:
- type: key
  id: Bunker
  effect: added
- type: key
  id: Bunkerable
  effect: added
- type: key
  id: BunkerDamageMultiplier
  effect: added
- type: key
  id: BunkerROFMultiplier
  effect: added
- type: key
  id: BunkerWeaponRangeBonus
  effect: added
- type: key
  id: BunkerWallsUpSound
  effect: added
- type: key
  id: BunkerWallsDownSound
  effect: added
- type: key
  id: OccupyHeight
  effect: added
- type: key
  id: Layer
  effect: added
- type: key
  id: PenetratesBunker
  effect: added
- type: system
  id: tank-bunkers
  effect: added
credit: [Lucas]
---

Vehicles can now drive into a Tank Bunker and fight from it with more damage, a faster rate of fire and longer range, shielded from all but `PenetratesBunker` warheads, as in Yuri's Revenge. Animations with `Layer=ground` in the art file now sort among the objects on the ground.
