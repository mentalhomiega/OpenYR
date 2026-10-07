---
key: SpecialAnimThreePoweredSpecial
summary: Marks the animation in special slot three as one a power plant removes in a blackout or a drain.
see_also: ["SpecialAnimThree", "SpecialAnimThreePoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SpecialAnimThreePoweredSpecial=` is read for special slot three, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`SpecialAnimThree`](/keys/specialanimthree/), [`SpecialAnimThreeDamaged`](/keys/specialanimthreedamaged/) or [`SpecialAnimThreeGarrisoned`](/keys/specialanimthreegarrisoned/).
