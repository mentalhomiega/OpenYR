---
key: SpecialAnimTwoPoweredSpecial
summary: A Yuri's Revenge power flag for special slot two that has no effect yet.
see_also: ["SpecialAnimTwo", "SpecialAnimTwoPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SpecialAnimTwoPoweredSpecial=` is read for special slot two, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`SpecialAnimTwo`](/keys/specialanimtwo/), [`SpecialAnimTwoDamaged`](/keys/specialanimtwodamaged/) or [`SpecialAnimTwoGarrisoned`](/keys/specialanimtwogarrisoned/).
