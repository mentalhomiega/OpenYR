---
key: LowPowerZAdjust
summary: The depth bias applied to the low power slot's animation.
see_also: ["LowPower", "LowPowerYSort"]
when_omitted:
  kind: value
  value: "0"
---

`LowPowerZAdjust=` decides whether the [`LowPower`](/keys/lowpower/) animation is drawn over the structure or behind it. A negative value brings the animation toward the viewer, so it covers the structure; a positive value pushes it back, so the structure covers it. The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
