---
title: Let each jumpjet type set its own flight
category: feature
release: 0.2.0
targets:
- type: key
  id: JumpjetTurnRate
  effect: added
- type: key
  id: JumpjetSpeed
  effect: added
- type: key
  id: JumpjetClimb
  effect: added
- type: key
  id: JumpjetHeight
  effect: added
- type: key
  id: JumpjetAccel
  effect: added
- type: key
  id: JumpjetWobbles
  effect: added
- type: key
  id: JumpjetDeviation
  effect: added
- type: key
  id: JumpjetNoWobbles
  effect: added
credit: [Lucas]
---

Jumpjet types now read their own speed, climb, height, acceleration, turn rate and bobbing, falling back to `[JumpjetControls]`, as in Yuri's Revenge. A Rocketeer that reaches its spot with a target now stops there and fires instead of hovering in place unable to shoot.
