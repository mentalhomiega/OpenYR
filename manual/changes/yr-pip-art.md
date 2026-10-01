---
title: Draw pips and rank insignia from the Yuri's Revenge pip art
category: fix
release: 0.2.0
targets:
- type: enum
  id: PipEnum
  effect: changed
- type: key
  id: Pip
  effect: changed
credit: [Lucas]
---

Veteran, elite and below-rookie insignia now use the Yuri's Revenge frames of `PIPS.SHP`, and the healer cross, which that file does not have, is no longer drawn. Pip colors accept the `person` figure names, and `empty` is no longer a pip color. A rules file that has a soldier's section but leaves out `Pip` now changes the color as Yuri's Revenge does, so a type that never sets `Pip` shows yellow passenger pips. Pips also show under the object beneath the mouse.
