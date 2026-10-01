---
key: ProductionAnimPoweredEffect
summary: Whether the production slot's animation is removed during a power shortfall and restored when power returns.
see_also: ["ProductionAnim", "ProductionAnimPowered", "ProductionAnimPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`ProductionAnim`](/keys/productionanim/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`ProductionAnimPoweredLight`](/keys/productionanimpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`ProductionAnimPowered`](/keys/productionanimpowered/) and [`ProductionAnimPoweredLight`](/keys/productionanimpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`ProductionAnim`](/keys/productionanim/), [`ProductionAnimDamaged`](/keys/productionanimdamaged/) or [`ProductionAnimGarrisoned`](/keys/productionanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
