---
key: SuperAnimTwoPoweredSpecial
summary: Marks the animation in super slot two as one a power plant removes in a blackout or a drain.
see_also: ["SuperAnimTwo", "SuperAnimTwoPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SuperAnimTwoPoweredSpecial=` is read for super slot two, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`SuperAnimTwo`](/keys/superanimtwo/), [`SuperAnimTwoDamaged`](/keys/superanimtwodamaged/) or [`SuperAnimTwoGarrisoned`](/keys/superanimtwogarrisoned/).
