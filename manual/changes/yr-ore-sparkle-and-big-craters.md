---
title: Hide ore sparkles and force big nuke craters
category: fix
release: 0.2.0
targets:
- type: key
  id: HideIfNoOre
  effect: added
- type: key
  id: ForceBigCraters
  effect: added
credit: [MentalHomiega]
---

An animation with `HideIfNoOre=yes` in its `art.ini` section is no longer drawn over a cell whose ore is gone, and it is drawn again when ore grows back, as in Yuri's Revenge. Before, ore sparkles such as `TWNK1` kept looping over bare ground.

An animation with `Crater=yes` and `ForceBigCraters=yes` in its `art.ini` section leaves a multiple-cell crater wherever one fits, and the stock nuke ball `NUKEBALL` now does so, as in Yuri's Revenge. Before, the size of the animation's artwork decided whether a multiple-cell crater could appear. A forced crater may also be laid on a cell that a building stands on.
