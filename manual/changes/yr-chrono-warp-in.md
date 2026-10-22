---
title: Draw a warping unit half see-through and stop it firing
category: fix
release: 0.2.0
targets:
- type: system
  id: temporal-weapons
  effect: changed
credit:
- MentalHomiega
---

A unit that a teleport or chronosphere warp is holding is drawn 50 percent translucent, and it cannot fire while it warps in or out.

The translucent draw covers both states. A unit warping out, during its chronosphere countdown, and a unit warping in, during the arrival hold after a teleport, are drawn half see-through. The unit is drawn fully opaque on the frame its hold ends. The draw has no gradual fade.

A unit warping in is refused every shot with the rearm result. A unit being warped out is refused with the illegal result. Both checks run before the weapon is chosen, so a unit with no weapon gets the same result.

A teleport order given during a hold waits until the hold ends.
