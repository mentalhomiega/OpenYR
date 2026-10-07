---
key: SuperAnimFourPoweredEffect
summary: Whether super slot four's animation is removed during a power shortfall and restored when power returns.
see_also: ["SuperAnimFour", "SuperAnimFourPowered", "SuperAnimFourPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SuperAnimFour`](/keys/superanimfour/) animation is removed while its house is short of power, and created again when the structure next comes back into service. Unlike [`SuperAnimFourPoweredLight`](/keys/superanimfourpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SuperAnimFourPowered`](/keys/superanimfourpowered/) and [`SuperAnimFourPoweredLight`](/keys/superanimfourpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SuperAnimFour`](/keys/superanimfour/), [`SuperAnimFourDamaged`](/keys/superanimfourdamaged/) or [`SuperAnimFourGarrisoned`](/keys/superanimfourgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
