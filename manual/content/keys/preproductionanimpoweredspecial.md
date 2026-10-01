---
key: PreProductionAnimPoweredSpecial
summary: A Yuri's Revenge power flag for the pre-production slot that has no effect yet.
see_also: ["PreProductionAnim", "PreProductionAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`PreProductionAnimPoweredSpecial=` is read for the pre-production slot, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`PreProductionAnim`](/keys/preproductionanim/), [`PreProductionAnimDamaged`](/keys/preproductionanimdamaged/) or [`PreProductionAnimGarrisoned`](/keys/preproductionanimgarrisoned/).
