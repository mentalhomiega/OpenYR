---
key: SuperLowPowerZAdjust
summary: The depth bias applied to the super low power slot's animation.
see_also: ["SuperLowPower", "SuperLowPowerYSort"]
when_omitted:
  kind: value
  value: "0"
---

`SuperLowPowerZAdjust=` decides whether the [`SuperLowPower`](/keys/superlowpower/) animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure; a positive value pushes it back, so the structure covers it. The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
