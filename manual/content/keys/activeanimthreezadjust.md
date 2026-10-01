---
key: ActiveAnimThreeZAdjust
summary: The depth bias applied to the third active slot's animation.
see_also: ["ActiveAnimThree", "ActiveAnimThreeYSort"]
when_omitted:
  kind: value
  value: "0"
---

`ActiveAnimThreeZAdjust` shifts the depth at which the [`ActiveAnimThree`](/keys/activeanimthree/) animation is drawn. A negative value brings the animation toward the viewer, and a large enough value draws it over the structure. A positive value pushes it back, and a large enough value lets the structure cover it.

[Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers how this depth bias differs from the sorting bias.
