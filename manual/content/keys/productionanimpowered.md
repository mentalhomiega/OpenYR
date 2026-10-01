---
key: ProductionAnimPowered
summary: Whether the production slot's animation freezes while its house is short of power.
see_also: ["ProductionAnim", "ProductionAnimPoweredLight", "ProductionAnimPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`ProductionAnim`](/keys/productionanim/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`ProductionAnimPoweredLight`](/keys/productionanimpoweredlight/) or [`ProductionAnimPoweredEffect`](/keys/productionanimpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`ProductionAnim`](/keys/productionanim/), [`ProductionAnimDamaged`](/keys/productionanimdamaged/) or [`ProductionAnimGarrisoned`](/keys/productionanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
