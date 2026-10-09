---
title: Land a jumpjet on its wanted height without overshooting
category: fix
release: 0.2.0
targets:
- type: key
  id: Climb
  effect: changed
- type: key
  id: JumpjetClimb
  effect: changed
credit: [MentalHomiega]
---

A jumpjet that is less than one climb step from the height it wants now moves the rest of the way in one frame and holds that height. Before, it stepped past the height and sank back on the next frame, so a hovering jumpjet bobbed by up to one step.
