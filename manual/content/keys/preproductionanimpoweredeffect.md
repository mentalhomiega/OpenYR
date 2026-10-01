---
key: PreProductionAnimPoweredEffect
summary: Whether the pre-production slot's animation is removed during a power shortfall and restored when power returns.
see_also: ["PreProductionAnim", "PreProductionAnimPowered", "PreProductionAnimPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`PreProductionAnim`](/keys/preproductionanim/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`PreProductionAnimPoweredLight`](/keys/preproductionanimpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`PreProductionAnimPowered`](/keys/preproductionanimpowered/) and [`PreProductionAnimPoweredLight`](/keys/preproductionanimpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`PreProductionAnim`](/keys/preproductionanim/), [`PreProductionAnimDamaged`](/keys/preproductionanimdamaged/) or [`PreProductionAnimGarrisoned`](/keys/preproductionanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
