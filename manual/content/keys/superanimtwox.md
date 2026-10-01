---
key: SuperAnimTwoX
summary: How far right of the structure's drawing point super slot two's animation sits, in screen pixels.
see_also: ["SuperAnimTwo", "SuperAnimTwoY"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimTwoX=` moves the [`SuperAnimTwo`](/keys/superanimtwo/) animation right of the structure's drawing point by that many screen pixels. A negative value moves it left. The value is read only when the slot has an animation name from [`SuperAnimTwo`](/keys/superanimtwo/), [`SuperAnimTwoDamaged`](/keys/superanimtwodamaged/) or [`SuperAnimTwoGarrisoned`](/keys/superanimtwogarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
