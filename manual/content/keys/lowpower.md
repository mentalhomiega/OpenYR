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

No event fills the low power slot yet; Yuri's Revenge fills it while a `PoweredSpecial=yes` structure's house is blacked out. The house's full-power pass can still start it through `LowPowerPowered=no` with `LowPowerPoweredLight=yes`.
