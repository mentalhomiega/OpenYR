---
key: ActiveAnimZAdjust
summary: The depth bias applied to the first active slot's animation.
see_also: ["ActiveAnim", "ActiveAnimYSort"]
when_omitted:
  kind: value
  value: "0"
---

The value decides whether active slot one's animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure. A positive value pushes it back, so the structure covers it.

The value is read only when the slot has an animation name from [`ActiveAnim`](/keys/activeanim/), [`ActiveAnimDamaged`](/keys/activeanimdamaged/) or [`ActiveAnimGarrisoned`](/keys/activeanimgarrisoned/).

[Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
