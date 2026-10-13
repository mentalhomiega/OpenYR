---
title: Draw occupied structures in their occupied frame and show garrison shots in front
category: fix
release: 0.2.0
targets:
- type: system
  id: garrisons
  effect: changed
- type: key
  id: OccupantAnim
  effect: changed
- type: key
  id: IdleAnimGarrisoned
  effect: changed
- type: key
  id: ActiveAnimGarrisoned
  effect: changed
credit: [MentalHomiega]
---

Before, an occupied structure kept the frame for its animation stage, and its garrison shots were drawn behind its walls. Now an occupied structure draws its occupied frame, and its idle and active animations run their garrisoned names while its health is above `ConditionYellow`. A garrison's shot is drawn in front of the structure. A weapon without `OccupantAnim` plays no animation for those shots.
