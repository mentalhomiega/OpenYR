---
key: SuperAnimZAdjust
summary: The depth bias applied to super slot one's animation.
see_also: ["SuperAnim", "SuperAnimYSort"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimZAdjust=` decides whether the [`SuperAnim`](/keys/superanim/) animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure; a positive value pushes it back, so the structure covers it. The value is read only when the slot has an animation name from [`SuperAnim`](/keys/superanim/), [`SuperAnimDamaged`](/keys/superanimdamaged/) or [`SuperAnimGarrisoned`](/keys/superanimgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
