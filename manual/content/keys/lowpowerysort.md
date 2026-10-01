---
key: LowPowerYSort
summary: The sorting bias applied to the low power slot's animation, in leptons.
see_also: ["LowPower", "LowPowerZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

`LowPowerYSort=` moves the [`LowPower`](/keys/lowpower/) animation earlier or later in the drawing order of the objects on its layer, in leptons. It has an effect only when the animation's AnimType sets [`Surface=yes`](/keys/surface/), and it replaces the AnimType's own [`YSortAdjust`](/keys/ysortadjust/). The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
