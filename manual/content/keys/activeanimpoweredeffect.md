---
key: ActiveAnimPoweredEffect
summary: Whether active slot one's animation is removed during a power shortfall and restored when power returns.
see_also: ["ActiveAnim", "ActiveAnimPowered", "ActiveAnimPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`ActiveAnim`](/keys/activeanim/) animation is removed while its house is short of power, and created again when the structure next comes back into service. Unlike [`ActiveAnimPoweredLight`](/keys/activeanimpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`ActiveAnimPowered`](/keys/activeanimpowered/) and [`ActiveAnimPoweredLight`](/keys/activeanimpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`ActiveAnim`](/keys/activeanim/), [`ActiveAnimDamaged`](/keys/activeanimdamaged/) or [`ActiveAnimGarrisoned`](/keys/activeanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
