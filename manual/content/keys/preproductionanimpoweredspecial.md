---
key: PreProductionAnimPoweredSpecial
summary: Marks the animation in the pre-production slot as one a power plant removes in a blackout or a drain.
see_also: ["PreProductionAnim", "PreProductionAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`PreProductionAnimPoweredSpecial=` is read for the pre-production slot, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`PreProductionAnim`](/keys/preproductionanim/), [`PreProductionAnimDamaged`](/keys/preproductionanimdamaged/) or [`PreProductionAnimGarrisoned`](/keys/preproductionanimgarrisoned/).
