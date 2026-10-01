---
key: SuperAnimY
summary: How far below the structure's drawing point super slot one's animation sits, in screen pixels.
see_also: ["SuperAnim", "SuperAnimX"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimY=` moves the [`SuperAnim`](/keys/superanim/) animation down from the structure's drawing point by that many screen pixels. A negative value moves it up. The value is read only when the slot has an animation name from [`SuperAnim`](/keys/superanim/), [`SuperAnimDamaged`](/keys/superanimdamaged/) or [`SuperAnimGarrisoned`](/keys/superanimgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
