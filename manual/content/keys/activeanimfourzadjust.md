---
key: ActiveAnimFourZAdjust
summary: The depth bias applied to the fourth active slot's animation.
see_also: ["ActiveAnimFour", "ActiveAnimFourYSort"]
when_omitted:
  kind: value
  value: "0"
---

Shifts the depth at which the [`ActiveAnimFour`](/keys/activeanimfour/) animation is drawn. A negative value brings the animation toward the viewer, so it is drawn over the structure. A positive value pushes it back, so the structure covers it.

[Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
