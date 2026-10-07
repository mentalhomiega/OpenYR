---
key: SpecialAnimTwoPoweredSpecial
summary: Marks the animation in special slot two as one a power plant removes in a blackout or a drain.
see_also: ["SpecialAnimTwo", "SpecialAnimTwoPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SpecialAnimTwoPoweredSpecial=` is read for special slot two, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`SpecialAnimTwo`](/keys/specialanimtwo/), [`SpecialAnimTwoDamaged`](/keys/specialanimtwodamaged/) or [`SpecialAnimTwoGarrisoned`](/keys/specialanimtwogarrisoned/).
