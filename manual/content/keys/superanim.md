---
key: SuperAnim
summary: The animation the structure runs in super slot one.
see_also: ["SuperAnimDamaged", "SuperAnimGarrisoned", "SuperAnimX", "SuperAnimY", "SuperAnimZAdjust", "SuperAnimYSort", "SuperAnimPowered", "system:building-animations"]
when_omitted:
  kind: value
  value: ""
---

`SuperAnim=` names the animation, registered in `[Animations]`, that the structure runs in super slot one. It is written in the structure's Image ID art entry. The animation is a separate object pinned to a point on the structure's artwork, playing at its own rate. [Building and emptying a slot](/systems/building-animations/#building-and-emptying-a-slot) covers what happens when the name is too long or not registered.

```ini title="artmd.ini"
[MYSTRUCT] ; example Image ID art entry
SuperAnim=MYANIM ; an AnimType registered in [Animations]
SuperAnimX=10
SuperAnimY=-20
```

A structure with a superweapon fills the super slots as its weapon charges, becomes ready and is fired; [`ChargedAnimTime`](/keys/chargedanimtime/) gives the order. A structure coming into service can also start the slot, which needs `SuperAnimPowered=no` with `SuperAnimPoweredLight=yes` on a [`Powered=yes`](/keys/powered/) structure that drains power.
