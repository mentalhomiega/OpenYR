---
key: SpecialAnimTwoPoweredEffect
summary: Whether special slot two's animation is removed during a power shortfall and restored when power returns.
see_also: ["SpecialAnimTwo", "SpecialAnimTwoPowered", "SpecialAnimTwoPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SpecialAnimTwo`](/keys/specialanimtwo/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`SpecialAnimTwoPoweredLight`](/keys/specialanimtwopoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SpecialAnimTwoPowered`](/keys/specialanimtwopowered/) and [`SpecialAnimTwoPoweredLight`](/keys/specialanimtwopoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SpecialAnimTwo`](/keys/specialanimtwo/), [`SpecialAnimTwoDamaged`](/keys/specialanimtwodamaged/) or [`SpecialAnimTwoGarrisoned`](/keys/specialanimtwogarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
