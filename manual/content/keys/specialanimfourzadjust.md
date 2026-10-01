---
key: SpecialAnimFourZAdjust
summary: The depth bias applied to special slot four's animation.
see_also: ["SpecialAnimFour", "SpecialAnimFourYSort"]
when_omitted:
  kind: value
  value: "0"
---

`SpecialAnimFourZAdjust=` decides whether the [`SpecialAnimFour`](/keys/specialanimfour/) animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure; a positive value pushes it back, so the structure covers it. The value is read only when the slot has an animation name from [`SpecialAnimFour`](/keys/specialanimfour/), [`SpecialAnimFourDamaged`](/keys/specialanimfourdamaged/) or [`SpecialAnimFourGarrisoned`](/keys/specialanimfourgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
