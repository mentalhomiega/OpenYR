---
key: ActiveAnimThreeYSort
summary: The sorting bias applied to the third active slot's animation, in leptons.
see_also: ["ActiveAnimThree", "ActiveAnimThreeZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

`ActiveAnimThreeYSort` moves the [`ActiveAnimThree`](/keys/activeanimthree/) animation in the drawing order of the ground layer, measured in leptons. A [cell](/glossary/#cell) is 256 leptons. A positive value moves the animation later in the order, so it is drawn after more of the objects around it. A negative value moves it earlier.

The value has no effect on an AnimType left at [`Surface=no`](/keys/surface/), because that animation is drawn in the air layer, which is not sorted.

The slot's value replaces the AnimType's [`YSortAdjust`](/keys/ysortadjust/). To keep that bias, repeat it in `ActiveAnimThreeYSort`.

[Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
