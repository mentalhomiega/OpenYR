---
key: SuperLowPower
summary: The animation the structure runs in the super low power slot.
see_also: ["SuperLowPowerDamaged", "SuperLowPowerGarrisoned", "SuperLowPowerX", "SuperLowPowerY", "SuperLowPowerZAdjust", "SuperLowPowerYSort", "SuperLowPowerPowered", "system:building-animations"]
when_omitted:
  kind: value
  value: ""
---

`SuperLowPower=` names the animation, registered in `[Animations]`, that the structure runs in the super low power slot. It is written in the structure's Image ID art entry. The animation is a separate object pinned to a point on the structure's artwork, playing at its own rate. [Building and emptying a slot](/systems/building-animations/#building-and-emptying-a-slot) covers what happens when the name is too long or not registered.

```ini title="artmd.ini"
[MYSTRUCT] ; example Image ID art entry
SuperLowPower=MYANIM ; an AnimType registered in [Animations]
SuperLowPowerX=10
SuperLowPowerY=-20
```

The slot starts when a power shortfall removes a [`SuperAnimThreePoweredEffect=yes`](/keys/superanimthreepoweredeffect/) animation, and empties when the house's power is full again.
