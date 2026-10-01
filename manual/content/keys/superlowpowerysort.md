---
key: SuperLowPowerYSort
summary: The sorting bias applied to the super low power slot's animation, in leptons.
see_also: ["SuperLowPower", "SuperLowPowerZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

`SuperLowPowerYSort=` moves the [`SuperLowPower`](/keys/superlowpower/) animation earlier or later in the drawing order of the objects on its layer, in leptons. It has an effect only when the animation's AnimType sets [`Surface=yes`](/keys/surface/), and it replaces the AnimType's own [`YSortAdjust`](/keys/ysortadjust/). The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
