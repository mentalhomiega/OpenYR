---
key: SuperAnimThreeX
summary: How far right of the structure's drawing point super slot three's animation sits, in screen pixels.
see_also: ["SuperAnimThree", "SuperAnimThreeY"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimThreeX=` moves the [`SuperAnimThree`](/keys/superanimthree/) animation right of the structure's drawing point by that many screen pixels. A negative value moves it left. The value is read only when the slot has an animation name from [`SuperAnimThree`](/keys/superanimthree/), [`SuperAnimThreeDamaged`](/keys/superanimthreedamaged/) or [`SuperAnimThreeGarrisoned`](/keys/superanimthreegarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
