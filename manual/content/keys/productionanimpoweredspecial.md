---
key: ProductionAnimPoweredSpecial
summary: A Yuri's Revenge power flag for the production slot that has no effect yet.
see_also: ["ProductionAnim", "ProductionAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ProductionAnimPoweredSpecial=` is read for the production slot, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`ProductionAnim`](/keys/productionanim/), [`ProductionAnimDamaged`](/keys/productionanimdamaged/) or [`ProductionAnimGarrisoned`](/keys/productionanimgarrisoned/).
