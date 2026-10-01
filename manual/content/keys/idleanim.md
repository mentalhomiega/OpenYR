---
key: IdleAnim
summary: The animation the structure runs in the idle slot.
see_also: ["IdleAnimDamaged", "IdleAnimGarrisoned", "IdleAnimX", "IdleAnimY", "IdleAnimZAdjust", "IdleAnimYSort", "IdleAnimPowered", "system:building-animations"]
when_omitted:
  kind: value
  value: ""
---

`IdleAnim=` names the animation, registered in `[Animations]`, that the structure runs in the idle slot. It is written in the structure's Image ID art entry. The animation is a separate object pinned to a point on the structure's artwork, playing at its own rate. [Building and emptying a slot](/systems/building-animations/#building-and-emptying-a-slot) covers what happens when the name is too long or not registered.

```ini title="artmd.ini"
[MYSTRUCT] ; example Image ID art entry
IdleAnim=MYANIM ; an AnimType registered in [Animations]
IdleAnimX=10
IdleAnimY=-20
```

The slot starts when the structure comes online, unless the structure is a [`Refinery=yes`](/keys/refinery/) type, and when the scenario places the structure. A non-looping animation empties the slot when it plays to its end.
