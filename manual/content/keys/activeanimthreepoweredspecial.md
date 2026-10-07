---
key: ActiveAnimThreePoweredSpecial
summary: Marks the animation in active slot three as one a power plant removes in a blackout or a drain.
see_also: ["ActiveAnimThree", "ActiveAnimThreePoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ActiveAnimThreePoweredSpecial=` is read for active slot three, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`ActiveAnimThree`](/keys/activeanimthree/), [`ActiveAnimThreeDamaged`](/keys/activeanimthreedamaged/) or [`ActiveAnimThreeGarrisoned`](/keys/activeanimthreegarrisoned/).
