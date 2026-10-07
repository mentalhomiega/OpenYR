---
key: SuperAnimThreePoweredEffect
summary: Whether super slot three's animation is removed during a power shortfall and restored when power returns.
see_also: ["SuperAnimThree", "SuperAnimThreePowered", "SuperAnimThreePoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SuperAnimThree`](/keys/superanimthree/) animation is removed while its house is short of power, and created again when the structure next comes back into service. Unlike [`SuperAnimThreePoweredLight`](/keys/superanimthreepoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SuperAnimThreePowered`](/keys/superanimthreepowered/) and [`SuperAnimThreePoweredLight`](/keys/superanimthreepoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SuperAnimThree`](/keys/superanimthree/), [`SuperAnimThreeDamaged`](/keys/superanimthreedamaged/) or [`SuperAnimThreeGarrisoned`](/keys/superanimthreegarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags. When super slot three's animation is removed this way, the structure starts its [`SuperLowPower`](/keys/superlowpower/) animation.
