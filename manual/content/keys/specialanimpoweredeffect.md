---
key: SpecialAnimPoweredEffect
summary: Whether special slot one's animation is removed during a power shortfall and restored when power returns.
see_also: ["SpecialAnim", "SpecialAnimPowered", "SpecialAnimPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SpecialAnim`](/keys/specialanim/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`SpecialAnimPoweredLight`](/keys/specialanimpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SpecialAnimPowered`](/keys/specialanimpowered/) and [`SpecialAnimPoweredLight`](/keys/specialanimpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SpecialAnim`](/keys/specialanim/), [`SpecialAnimDamaged`](/keys/specialanimdamaged/) or [`SpecialAnimGarrisoned`](/keys/specialanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
