---
key: SuperAnimTwoZAdjust
summary: The depth bias applied to super slot two's animation.
see_also: ["SuperAnimTwo", "SuperAnimTwoYSort"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimTwoZAdjust=` decides whether the [`SuperAnimTwo`](/keys/superanimtwo/) animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure; a positive value pushes it back, so the structure covers it. The value is read only when the slot has an animation name from [`SuperAnimTwo`](/keys/superanimtwo/), [`SuperAnimTwoDamaged`](/keys/superanimtwodamaged/) or [`SuperAnimTwoGarrisoned`](/keys/superanimtwogarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
