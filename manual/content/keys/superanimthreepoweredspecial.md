---
key: SuperAnimThreePoweredSpecial
summary: Marks the animation in super slot three as one a power plant removes in a blackout or a drain.
see_also: ["SuperAnimThree", "SuperAnimThreePoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SuperAnimThreePoweredSpecial=` is read for super slot three, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`SuperAnimThree`](/keys/superanimthree/), [`SuperAnimThreeDamaged`](/keys/superanimthreedamaged/) or [`SuperAnimThreeGarrisoned`](/keys/superanimthreegarrisoned/).
