---
key: SpecialAnimFourPoweredEffect
summary: Whether special slot four's animation is removed during a power shortfall and restored when power returns.
see_also: ["SpecialAnimFour", "SpecialAnimFourPowered", "SpecialAnimFourPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SpecialAnimFour`](/keys/specialanimfour/) animation is removed while its house is short of power, and created again when the structure next comes back into service. Unlike [`SpecialAnimFourPoweredLight`](/keys/specialanimfourpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SpecialAnimFourPowered`](/keys/specialanimfourpowered/) and [`SpecialAnimFourPoweredLight`](/keys/specialanimfourpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SpecialAnimFour`](/keys/specialanimfour/), [`SpecialAnimFourDamaged`](/keys/specialanimfourdamaged/) or [`SpecialAnimFourGarrisoned`](/keys/specialanimfourgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
