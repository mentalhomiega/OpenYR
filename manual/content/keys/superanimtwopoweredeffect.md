---
key: SuperAnimTwoPoweredEffect
summary: Whether super slot two's animation is removed during a power shortfall and restored when power returns.
see_also: ["SuperAnimTwo", "SuperAnimTwoPowered", "SuperAnimTwoPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SuperAnimTwo`](/keys/superanimtwo/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`SuperAnimTwoPoweredLight`](/keys/superanimtwopoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SuperAnimTwoPowered`](/keys/superanimtwopowered/) and [`SuperAnimTwoPoweredLight`](/keys/superanimtwopoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SuperAnimTwo`](/keys/superanimtwo/), [`SuperAnimTwoDamaged`](/keys/superanimtwodamaged/) or [`SuperAnimTwoGarrisoned`](/keys/superanimtwogarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
