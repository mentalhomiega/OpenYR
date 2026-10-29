---
title: Clear and check a chronosphere landing when the unit lands
category: fix
release: 0.2.0
targets:
- type: key
  id: ChronoDelay
  effect: changed
- type: key
  id: WarpOut
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A unit that a chronosphere moves now leaves its cell 60 frames after the chronosphere fires, and whatever stands on its destination is destroyed then, not at the shot. A vehicle that drives onto the destination during the wait is destroyed. An Iron Curtained unit on the destination destroys the arriving unit when it leaves, and one on the ground below a bridge destroys it when it lands. A soldier destroys only the soldiers on its own spot. Units warped together no longer spare each other. The warp-out animation and sound play when the unit leaves, and the landing plays `WarpOut=` instead of `WarpIn=`.

`ChronoDelay` no longer sets the wait. It is the hold a unit keeps after a landing that had to move to another cell, and the unit keeps it for later warps.
