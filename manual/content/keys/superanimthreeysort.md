---
key: SuperAnimThreeYSort
summary: The sorting bias applied to super slot three's animation, in leptons.
see_also: ["SuperAnimThree", "SuperAnimThreeZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimThreeYSort=` moves the [`SuperAnimThree`](/keys/superanimthree/) animation earlier or later in the drawing order of the objects on its layer, in leptons. It has an effect only when the animation's AnimType sets [`Surface=yes`](/keys/surface/), and it replaces the AnimType's own [`YSortAdjust`](/keys/ysortadjust/). The value is read only when the slot has an animation name from [`SuperAnimThree`](/keys/superanimthree/), [`SuperAnimThreeDamaged`](/keys/superanimthreedamaged/) or [`SuperAnimThreeGarrisoned`](/keys/superanimthreegarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
