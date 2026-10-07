---
key: LowPower
summary: The animation the structure runs in the low power slot.
see_also: ["LowPowerDamaged", "LowPowerGarrisoned", "LowPowerX", "LowPowerY", "LowPowerZAdjust", "LowPowerYSort", "LowPowerPowered", "system:building-animations"]
when_omitted:
  kind: value
  value: ""
---

`LowPower=` names the animation, registered in `[Animations]`, that the structure runs in the low power slot. It is written in the structure's Image ID art entry. The animation is a separate object pinned to a point on the structure's artwork, playing at its own rate. [Building and emptying a slot](/systems/building-animations/#building-and-emptying-a-slot) covers what happens when the name is too long or not registered.

```ini title="artmd.ini"
[MYSTRUCT] ; example Image ID art entry
LowPower=MYANIM ; an AnimType registered in [Animations]
LowPowerX=10
LowPowerY=-20
```

A [`PoweredSpecial=yes`](/keys/poweredspecial/) structure fills the low power slot while it is out of service in a spy's blackout or a power drain, and empties it when it works again. A structure coming into service can still start it through `LowPowerPowered=no` with `LowPowerPoweredLight=yes`.
