---
key: SpecialAnimThreePoweredEffect
summary: Whether special slot three's animation is removed during a power shortfall and restored when power returns.
see_also: ["SpecialAnimThree", "SpecialAnimThreePowered", "SpecialAnimThreePoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SpecialAnimThree`](/keys/specialanimthree/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`SpecialAnimThreePoweredLight`](/keys/specialanimthreepoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SpecialAnimThreePowered`](/keys/specialanimthreepowered/) and [`SpecialAnimThreePoweredLight`](/keys/specialanimthreepoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SpecialAnimThree`](/keys/specialanimthree/), [`SpecialAnimThreeDamaged`](/keys/specialanimthreedamaged/) or [`SpecialAnimThreeGarrisoned`](/keys/specialanimthreegarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
