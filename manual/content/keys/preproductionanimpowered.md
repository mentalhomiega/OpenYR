---
key: PreProductionAnimPowered
summary: Whether the pre-production slot's animation freezes while its house is short of power.
see_also: ["PreProductionAnim", "PreProductionAnimPoweredLight", "PreProductionAnimPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`PreProductionAnim`](/keys/preproductionanim/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`PreProductionAnimPoweredLight`](/keys/preproductionanimpoweredlight/) or [`PreProductionAnimPoweredEffect`](/keys/preproductionanimpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`PreProductionAnim`](/keys/preproductionanim/), [`PreProductionAnimDamaged`](/keys/preproductionanimdamaged/) or [`PreProductionAnimGarrisoned`](/keys/preproductionanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
