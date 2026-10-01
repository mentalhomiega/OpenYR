---
key: ActiveAnimFourYSort
summary: The sorting bias applied to the fourth active slot's animation, in leptons.
see_also: ["ActiveAnimFour", "ActiveAnimFourZAdjust"]
when_omitted:
  kind: value
  value: "0"
---

Adds this many leptons to the [`ActiveAnimFour`](/keys/activeanimfour/) animation's position in the ground layer's drawing order; a cell is 256 leptons. A positive value moves the animation later in that order and a negative value moves it earlier. The value has no effect on an animation drawn in the air layer, which includes every AnimType left at the default [`Surface=no`](/keys/surface/).

The slot's value replaces the AnimType's own [`YSortAdjust=`](/keys/ysortadjust/), so repeat that value here to keep it.

[Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this sorting bias differs from the depth bias.
