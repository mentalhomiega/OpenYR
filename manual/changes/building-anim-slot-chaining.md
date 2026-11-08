---
title: Chain building animation slots as in Yuri's Revenge
category: fix
release: 0.2.0
targets:
- type: key
  id: SpecialAnim
  effect: changed
- type: key
  id: SpecialAnimThree
  effect: changed
- type: key
  id: ChargedAnimTime
  effect: changed
credit:
- MentalHomiega
---

A structure with a superweapon now starts [`SuperAnimThree`](/keys/superanimthree/) when its [`SuperAnimTwo`](/keys/superanimtwo/) ends, and [`SuperAnim`](/keys/superanim/) when its [`SuperAnimFour`](/keys/superanimfour/) ends. Before, the structure showed nothing between the end of `SuperAnimTwo` and the weapon being ready. A service depot that loses its vehicle while the doors open starts its [`IdleAnim`](/keys/idleanim/) when [`SpecialAnim`](/keys/specialanim/) ends, and a delayed-fire structure only restarts its active animation when `SpecialAnim` plays to its end. The idle animation after [`SpecialAnimThree`](/keys/specialanimthree/) now starts even when a new repair has begun.
