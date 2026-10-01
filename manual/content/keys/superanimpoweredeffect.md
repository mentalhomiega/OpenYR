---
key: SuperAnimPoweredEffect
summary: Whether super slot one's animation is removed during a power shortfall and restored when power returns.
see_also: ["SuperAnim", "SuperAnimPowered", "SuperAnimPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SuperAnim`](/keys/superanim/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`SuperAnimPoweredLight`](/keys/superanimpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SuperAnimPowered`](/keys/superanimpowered/) and [`SuperAnimPoweredLight`](/keys/superanimpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SuperAnim`](/keys/superanim/), [`SuperAnimDamaged`](/keys/superanimdamaged/) or [`SuperAnimGarrisoned`](/keys/superanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
