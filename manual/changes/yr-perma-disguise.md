---
title: Keep spies permanently disguised and strip disguises on damage
category: feature
release: 0.2.0
targets:
- type: system
  id: disguises
  effect: changed
- type: key
  id: PermaDisguise
  effect: added
- type: key
  id: AlliedDisguise
  effect: added
- type: key
  id: SovietDisguise
  effect: added
- type: key
  id: ThirdDisguise
  effect: added
credit: [Lucas]
---

A `PermaDisguise=yes` spy is now always disguised as its side's default soldier, and any other disguise is lost when its wearer is hurt, as in Yuri's Revenge.
