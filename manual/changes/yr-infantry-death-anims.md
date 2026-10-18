---
title: Replace a soldier's death with its DeathAnims list
category: feature
release: 0.2.0
targets:
- type: key
  id: DeathAnims
  effect: added
- type: key
  id: Sequence
  effect: changed
credit:
- MentalHomiega
---

An infantry type with `DeathAnims=` now dies with the list entry at the position its warhead's `InfDeath` value gives, as in Yuri's Revenge. The soldier is removed at once, with no death sequence or corpse. Before, the key was ignored.

An art sequence's `<Name>Sounds=` key now plays a sound on a frame of that run, as in Yuri's Revenge; the Slave's whip on the third frame of `FireUp` is one example. Before, these sounds never played.

Saved games made before this build may not load, because infantry types now store these lists and sounds.
