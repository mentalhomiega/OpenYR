---
key: SuperAnimThree
summary: The animation the structure runs in super slot three.
see_also: ["SuperAnimThreeDamaged", "SuperAnimThreeGarrisoned", "SuperAnimThreeX", "SuperAnimThreeY", "SuperAnimThreeZAdjust", "SuperAnimThreeYSort", "SuperAnimThreePowered", "system:building-animations"]
when_omitted:
  kind: value
  value: ""
---

`SuperAnimThree=` names the animation, registered in `[Animations]`, that the structure runs in super slot three. It is written in the structure's Image ID art entry. The animation is a separate object pinned to a point on the structure's artwork, playing at its own rate. [Building and emptying a slot](/systems/building-animations/#building-and-emptying-a-slot) covers what happens when the name is too long or not registered.

```ini title="artmd.ini"
[MYSTRUCT] ; example Image ID art entry
SuperAnimThree=MYANIM ; an AnimType registered in [Animations]
SuperAnimThreeX=10
SuperAnimThreeY=-20
```

A structure with a superweapon fills the super slots as its weapon charges, becomes ready and is fired; [`ChargedAnimTime`](/keys/chargedanimtime/) gives the order. The house's full-power pass can also start the slot, which needs `SuperAnimThreePowered=no` with `SuperAnimThreePoweredLight=yes` on a [`Powered=yes`](/keys/powered/) structure that drains power.
