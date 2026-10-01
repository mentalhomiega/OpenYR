---
key: SpecialAnimFourYSort
summary: The sorting bias applied to special slot four's animation, in leptons.
see_also: ["SpecialAnimFour", "SpecialAnimFourZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

`SpecialAnimFourYSort=` moves the [`SpecialAnimFour`](/keys/specialanimfour/) animation earlier or later in the drawing order of the objects on its layer, in leptons. It has an effect only when the animation's AnimType sets [`Surface=yes`](/keys/surface/), and it replaces the AnimType's own [`YSortAdjust`](/keys/ysortadjust/). The value is read only when the slot has an animation name from [`SpecialAnimFour`](/keys/specialanimfour/), [`SpecialAnimFourDamaged`](/keys/specialanimfourdamaged/) or [`SpecialAnimFourGarrisoned`](/keys/specialanimfourgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
