---
title: Let engineers disarm Ivan bombs
category: feature
release: 0.2.0
targets:
- type: system
  id: ivan-bombs
  effect: changed
- type: key
  id: BombDisarm
  effect: added
- type: key
  id: BombSight
  effect: added
credit: [MentalHomiega]
---

Engineers now show the disarm cursor over bombs the player sees and remove them with a `BombDisarm=yes` warhead, and `BombSight` objects show the player nearby bombs, as in Yuri's Revenge.
