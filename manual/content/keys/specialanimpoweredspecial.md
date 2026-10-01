---
key: SpecialAnimPoweredSpecial
summary: A Yuri's Revenge power flag for special slot one that has no effect yet.
see_also: ["SpecialAnim", "SpecialAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SpecialAnimPoweredSpecial=` is read for special slot one, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`SpecialAnim`](/keys/specialanim/), [`SpecialAnimDamaged`](/keys/specialanimdamaged/) or [`SpecialAnimGarrisoned`](/keys/specialanimgarrisoned/).
