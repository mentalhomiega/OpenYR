---
key: SuperAnimYSort
summary: The sorting bias applied to super slot one's animation, in leptons.
see_also: ["SuperAnim", "SuperAnimZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

`SuperAnimYSort=` moves the [`SuperAnim`](/keys/superanim/) animation earlier or later in the drawing order of the objects on its layer, in leptons. It has an effect only when the animation's AnimType sets [`Surface=yes`](/keys/surface/), and it replaces the AnimType's own [`YSortAdjust`](/keys/ysortadjust/). The value is read only when the slot has an animation name from [`SuperAnim`](/keys/superanim/), [`SuperAnimDamaged`](/keys/superanimdamaged/) or [`SuperAnimGarrisoned`](/keys/superanimgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
