---
key: SuperAnimFour
summary: The animation the structure runs in super slot four.
see_also: ["SuperAnimFourDamaged", "SuperAnimFourGarrisoned", "SuperAnimFourX", "SuperAnimFourY", "SuperAnimFourZAdjust", "SuperAnimFourYSort", "SuperAnimFourPowered", "system:building-animations"]
when_omitted:
  kind: value
  value: ""
---

`SuperAnimFour=` names the animation, registered in `[Animations]`, that the structure runs in super slot four. It is written in the structure's Image ID art entry. The animation is a separate object pinned to a point on the structure's artwork, playing at its own rate. [Building and emptying a slot](/systems/building-animations/#building-and-emptying-a-slot) covers what happens when the name is too long or not registered.

```ini title="artmd.ini"
[MYSTRUCT] ; example Image ID art entry
SuperAnimFour=MYANIM ; an AnimType registered in [Animations]
SuperAnimFourX=10
SuperAnimFourY=-20
```

A structure with a superweapon fills the super slots as its weapon charges, becomes ready and is fired; [`ChargedAnimTime`](/keys/chargedanimtime/) gives the order. A structure coming into service can also start the slot, which needs `SuperAnimFourPowered=no` with `SuperAnimFourPoweredLight=yes` on a [`Powered=yes`](/keys/powered/) structure that drains power.
