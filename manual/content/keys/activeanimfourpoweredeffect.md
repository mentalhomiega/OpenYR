---
key: ActiveAnimFourPoweredEffect
summary: Whether active slot four's animation is removed during a power shortfall and restored when power returns.
see_also: ["ActiveAnimFour", "ActiveAnimFourPowered", "ActiveAnimFourPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`ActiveAnimFour`](/keys/activeanimfour/) animation is removed while its house is short of power, and created again when the structure next comes back into service. Unlike [`ActiveAnimFourPoweredLight`](/keys/activeanimfourpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`ActiveAnimFourPowered`](/keys/activeanimfourpowered/) and [`ActiveAnimFourPoweredLight`](/keys/activeanimfourpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`ActiveAnimFour`](/keys/activeanimfour/), [`ActiveAnimFourDamaged`](/keys/activeanimfourdamaged/) or [`ActiveAnimFourGarrisoned`](/keys/activeanimfourgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
