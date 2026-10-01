---
key: SuperLowPowerY
summary: How far below the structure's drawing point the super low power slot's animation sits, in screen pixels.
see_also: ["SuperLowPower", "SuperLowPowerX"]
when_omitted:
  kind: value
  value: "0"
---

`SuperLowPowerY=` moves the [`SuperLowPower`](/keys/superlowpower/) animation down from the structure's drawing point by that many screen pixels. A negative value moves it up. The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
