---
key: ActiveAnimThreePoweredEffect
summary: Whether active slot three's animation is removed during a power shortfall and restored when power returns.
see_also: ["ActiveAnimThree", "ActiveAnimThreePowered", "ActiveAnimThreePoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`ActiveAnimThree`](/keys/activeanimthree/) animation is removed while its house is short of power, and created again when the structure next comes back into service. Unlike [`ActiveAnimThreePoweredLight`](/keys/activeanimthreepoweredlight/), it returns only if the shortfall removed it. The flag works only when [`ActiveAnimThreePowered`](/keys/activeanimthreepowered/) and [`ActiveAnimThreePoweredLight`](/keys/activeanimthreepoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`ActiveAnimThree`](/keys/activeanimthree/), [`ActiveAnimThreeDamaged`](/keys/activeanimthreedamaged/) or [`ActiveAnimThreeGarrisoned`](/keys/activeanimthreegarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
