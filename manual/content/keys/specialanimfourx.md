---
key: SpecialAnimFourX
summary: How far right of the structure's drawing point special slot four's animation sits, in screen pixels.
see_also: ["SpecialAnimFour", "SpecialAnimFourY"]
when_omitted:
  kind: value
  value: "0"
---

`SpecialAnimFourX=` moves the [`SpecialAnimFour`](/keys/specialanimfour/) animation right of the structure's drawing point by that many screen pixels. A negative value moves it left. The value is read only when the slot has an animation name from [`SpecialAnimFour`](/keys/specialanimfour/), [`SpecialAnimFourDamaged`](/keys/specialanimfourdamaged/) or [`SpecialAnimFourGarrisoned`](/keys/specialanimfourgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
