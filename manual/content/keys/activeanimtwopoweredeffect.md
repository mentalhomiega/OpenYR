---
key: ActiveAnimTwoPoweredEffect
summary: Whether active slot two's animation is removed during a power shortfall and restored when power returns.
see_also: ["ActiveAnimTwo", "ActiveAnimTwoPowered", "ActiveAnimTwoPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`ActiveAnimTwo`](/keys/activeanimtwo/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`ActiveAnimTwoPoweredLight`](/keys/activeanimtwopoweredlight/), it returns only if the shortfall removed it. The flag works only when [`ActiveAnimTwoPowered`](/keys/activeanimtwopowered/) and [`ActiveAnimTwoPoweredLight`](/keys/activeanimtwopoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`ActiveAnimTwo`](/keys/activeanimtwo/), [`ActiveAnimTwoDamaged`](/keys/activeanimtwodamaged/) or [`ActiveAnimTwoGarrisoned`](/keys/activeanimtwogarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
