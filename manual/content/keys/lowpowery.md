---
key: LowPowerY
summary: How far below the structure's drawing point the low power slot's animation sits, in screen pixels.
see_also: ["LowPower", "LowPowerX"]
when_omitted:
  kind: value
  value: "0"
---

`LowPowerY=` moves the [`LowPower`](/keys/lowpower/) animation down from the structure's drawing point by that many screen pixels. A negative value moves it up. The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
