---
title: Spread blast damage as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: key
  id: CellSpread
  effect: added
- type: key
  id: PercentAtMax
  effect: added
- type: key
  id: WallAbsoluteDestroyer
  effect: added
- type: key
  id: DamageSelf
  effect: added
- type: key
  id: Spread
  scope: warheadtype
  effect: changed
- type: key
  id: MinDamage
  effect: changed
- type: key
  id: Verses
  effect: changed
- type: key
  id: ChainReaction
  effect: changed
- type: system
  id: warheads
  effect: changed
credit: [Lucas]
---

A blast now reaches as far as its warhead's `CellSpread` and loses damage in a straight line to `PercentAtMax` at that distance, as in Yuri's Revenge. Before, every blast reached a cell and a half and thinned by `Spread`. A blast hits a large structure once for each of its cells in reach, knocks down walls throughout its reach, and no longer sets off Tiberium. `MinDamage` has no effect, so a hit can come to nothing.
