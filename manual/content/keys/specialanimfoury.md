---
key: SpecialAnimFourY
summary: How far below the structure's drawing point special slot four's animation sits, in screen pixels.
see_also: ["SpecialAnimFour", "SpecialAnimFourX"]
when_omitted:
  kind: value
  value: "0"
---

`SpecialAnimFourY=` moves the [`SpecialAnimFour`](/keys/specialanimfour/) animation down from the structure's drawing point by that many screen pixels. A negative value moves it up. The value is read only when the slot has an animation name from [`SpecialAnimFour`](/keys/specialanimfour/), [`SpecialAnimFourDamaged`](/keys/specialanimfourdamaged/) or [`SpecialAnimFourGarrisoned`](/keys/specialanimfourgarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
