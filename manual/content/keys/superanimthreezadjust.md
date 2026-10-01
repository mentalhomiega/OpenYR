---
key: SuperAnimThreeZAdjust
summary: The depth bias applied to super slot three's animation.
see_also: ["SuperAnimThree", "SuperAnimThreeYSort"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimThreeZAdjust=` decides whether the [`SuperAnimThree`](/keys/superanimthree/) animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure; a positive value pushes it back, so the structure covers it. The value is read only when the slot has an animation name from [`SuperAnimThree`](/keys/superanimthree/), [`SuperAnimThreeDamaged`](/keys/superanimthreedamaged/) or [`SuperAnimThreeGarrisoned`](/keys/superanimthreegarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
