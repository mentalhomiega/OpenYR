---
key: SpecialAnimThreeYSort
summary: The sorting bias applied to the third special slot's animation, in leptons.
see_also: ["SpecialAnimThree", "SpecialAnimThreeZAdjust", "SpecialAnim"]
when_omitted:
  kind: value
  value: "0"
---

The value moves the third special slot's animation within the draw order of the ground layer, in leptons; a [cell](/glossary/#cell) is 256 leptons. A larger value sorts the animation later among the objects around it, and a smaller value sorts it earlier. An AnimType left at [`Surface=no`](/keys/surface/) is drawn in the air layer, which is not sorted, so the value has no effect on it.

The value replaces the AnimType's own [`YSortAdjust`](/keys/ysortadjust/). It is read only when the slot has an animation name from [`SpecialAnimThree`](/keys/specialanimthree/), [`SpecialAnimThreeDamaged`](/keys/specialanimthreedamaged/) or [`SpecialAnimThreeGarrisoned`](/keys/specialanimthreegarrisoned/). Write it in the same art entry as the animation names. [Where each setting is read from](/systems/building-animations/#where-each-setting-is-read-from) has the full table.

[Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
