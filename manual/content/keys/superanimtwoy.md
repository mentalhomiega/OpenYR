---
key: SuperAnimTwoY
summary: How far below the structure's drawing point super slot two's animation sits, in screen pixels.
see_also: ["SuperAnimTwo", "SuperAnimTwoX"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimTwoY=` moves the [`SuperAnimTwo`](/keys/superanimtwo/) animation down from the structure's drawing point by that many screen pixels. A negative value moves it up. The value is read only when the slot has an animation name from [`SuperAnimTwo`](/keys/superanimtwo/), [`SuperAnimTwoDamaged`](/keys/superanimtwodamaged/) or [`SuperAnimTwoGarrisoned`](/keys/superanimtwogarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
