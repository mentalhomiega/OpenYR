---
key: SpecialAnimFour
summary: The animation the structure runs in special slot four.
see_also: ["SpecialAnimFourDamaged", "SpecialAnimFourGarrisoned", "SpecialAnimFourX", "SpecialAnimFourY", "SpecialAnimFourZAdjust", "SpecialAnimFourYSort", "SpecialAnimFourPowered", "system:building-animations"]
when_omitted:
  kind: value
  value: ""
---

`SpecialAnimFour=` names the animation, registered in `[Animations]`, that the structure runs in special slot four. It is written in the structure's Image ID art entry. The animation is a separate object pinned to a point on the structure's artwork, playing at its own rate. [Building and emptying a slot](/systems/building-animations/#building-and-emptying-a-slot) covers what happens when the name is too long or not registered.

```ini title="artmd.ini"
[MYSTRUCT] ; example Image ID art entry
SpecialAnimFour=MYANIM ; an AnimType registered in [Animations]
SpecialAnimFourX=10
SpecialAnimFourY=-20
```

No event fills special slot four. The slot runs only when the house's full-power pass starts it, which needs `SpecialAnimFourPowered=no` with `SpecialAnimFourPoweredLight=yes` on a [`Powered=yes`](/keys/powered/) structure that drains power.
