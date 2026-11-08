---
title: Keep SuperAnimTwo playing when a superweapon fires
category: fix
release: 0.2.0
targets:
- type: key
  id: SuperAnimTwo
  effect: changed
- type: key
  id: SuperAnimFour
  effect: changed
credit:
- MentalHomiega
---

A structure whose superweapon fires while [`SuperAnimTwo`](/keys/superanimtwo/) is still playing now lets it play to its end, as Yuri's Revenge does. [`SuperAnimThree`](/keys/superanimthree/) follows it for one update, and then [`SuperAnimFour`](/keys/superanimfour/) plays. Before, firing ended `SuperAnimTwo` at once and started `SuperAnimFour`. Firing while `SuperAnimThree` plays is unchanged.
