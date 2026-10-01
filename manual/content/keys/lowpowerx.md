---
key: LowPowerX
summary: How far right of the structure's drawing point the low power slot's animation sits, in screen pixels.
see_also: ["LowPower", "LowPowerY"]
when_omitted:
  kind: value
  value: "0"
---

`LowPowerX=` moves the [`LowPower`](/keys/lowpower/) animation right of the structure's drawing point by that many screen pixels. A negative value moves it left. The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/). [Placement and draw order](/systems/building-animations/#placement-and-draw-order) covers the offset.
