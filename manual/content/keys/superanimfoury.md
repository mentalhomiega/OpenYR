---
key: SuperAnimFourY
summary: How far below the structure's drawing point super slot four's animation sits, in screen pixels.
see_also: ["SuperAnimFour", "SuperAnimFourX"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimFourY=` moves the [`SuperAnimFour`](/keys/superanimfour/) animation down from the structure's drawing point by that many screen pixels. A negative value moves it up. The value is read only when the slot has an animation name from [`SuperAnimFour`](/keys/superanimfour/), [`SuperAnimFourDamaged`](/keys/superanimfourdamaged/) or [`SuperAnimFourGarrisoned`](/keys/superanimfourgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
