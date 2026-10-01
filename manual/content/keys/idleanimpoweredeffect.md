---
key: IdleAnimPoweredEffect
summary: Whether the idle slot's animation is removed during a power shortfall and restored when power returns.
see_also: ["IdleAnim", "IdleAnimPowered", "IdleAnimPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`IdleAnim`](/keys/idleanim/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`IdleAnimPoweredLight`](/keys/idleanimpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`IdleAnimPowered`](/keys/idleanimpowered/) and [`IdleAnimPoweredLight`](/keys/idleanimpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`IdleAnim`](/keys/idleanim/), [`IdleAnimDamaged`](/keys/idleanimdamaged/) or [`IdleAnimGarrisoned`](/keys/idleanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
