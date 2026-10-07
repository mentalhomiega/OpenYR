---
key: SuperAnimPoweredSpecial
summary: Marks the animation in super slot one as one a power plant removes in a blackout or a drain.
see_also: ["SuperAnim", "SuperAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SuperAnimPoweredSpecial=` is read for super slot one, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`SuperAnim`](/keys/superanim/), [`SuperAnimDamaged`](/keys/superanimdamaged/) or [`SuperAnimGarrisoned`](/keys/superanimgarrisoned/).
