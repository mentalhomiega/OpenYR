---
key: IdleAnimYSort
summary: The sorting bias applied to the idle slot's animation, in leptons.
see_also: ["IdleAnim", "IdleAnimZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

`IdleAnimYSort=` moves the [`IdleAnim`](/keys/idleanim/) animation earlier or later in the drawing order of the objects on its layer, in leptons. It has an effect only when the animation's AnimType sets [`Surface=yes`](/keys/surface/), and it replaces the AnimType's own [`YSortAdjust`](/keys/ysortadjust/). The value is read only when the slot has an animation name from [`IdleAnim`](/keys/idleanim/), [`IdleAnimDamaged`](/keys/idleanimdamaged/) or [`IdleAnimGarrisoned`](/keys/idleanimgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
