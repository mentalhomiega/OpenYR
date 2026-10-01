---
key: SuperAnimFourX
summary: How far right of the structure's drawing point super slot four's animation sits, in screen pixels.
see_also: ["SuperAnimFour", "SuperAnimFourY"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimFourX=` moves the [`SuperAnimFour`](/keys/superanimfour/) animation right of the structure's drawing point by that many screen pixels. A negative value moves it left. The value is read only when the slot has an animation name from [`SuperAnimFour`](/keys/superanimfour/), [`SuperAnimFourDamaged`](/keys/superanimfourdamaged/) or [`SuperAnimFourGarrisoned`](/keys/superanimfourgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
