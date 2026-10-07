---
key: ProductionAnimPoweredSpecial
summary: Marks the animation in the production slot as one a power plant removes in a blackout or a drain.
see_also: ["ProductionAnim", "ProductionAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ProductionAnimPoweredSpecial=` is read for the production slot, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`ProductionAnim`](/keys/productionanim/), [`ProductionAnimDamaged`](/keys/productionanimdamaged/) or [`ProductionAnimGarrisoned`](/keys/productionanimgarrisoned/).
