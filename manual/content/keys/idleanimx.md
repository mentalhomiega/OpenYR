---
key: IdleAnimX
summary: How far right of the structure's drawing point the idle slot's animation sits, in screen pixels.
see_also: ["IdleAnim", "IdleAnimY"]
when_omitted:
  kind: value
  value: "0"
---

`IdleAnimX=` moves the [`IdleAnim`](/keys/idleanim/) animation right of the structure's drawing point by that many screen pixels. A negative value moves it left. The value is read only when the slot has an animation name from [`IdleAnim`](/keys/idleanim/), [`IdleAnimDamaged`](/keys/idleanimdamaged/) or [`IdleAnimGarrisoned`](/keys/idleanimgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
