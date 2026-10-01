---
key: IdleAnimZAdjust
summary: The depth bias applied to the idle slot's animation.
see_also: ["IdleAnim", "IdleAnimYSort"]
when_omitted:
  kind: value
  value: "0"
---

`IdleAnimZAdjust=` decides whether the [`IdleAnim`](/keys/idleanim/) animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure; a positive value pushes it back, so the structure covers it. The value is read only when the slot has an animation name from [`IdleAnim`](/keys/idleanim/), [`IdleAnimDamaged`](/keys/idleanimdamaged/) or [`IdleAnimGarrisoned`](/keys/idleanimgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
