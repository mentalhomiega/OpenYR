---
key: ActiveAnimFourPoweredSpecial
summary: Marks the animation in active slot four as one a power plant removes in a blackout or a drain.
see_also: ["ActiveAnimFour", "ActiveAnimFourPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ActiveAnimFourPoweredSpecial=` is read for active slot four, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`ActiveAnimFour`](/keys/activeanimfour/), [`ActiveAnimFourDamaged`](/keys/activeanimfourdamaged/) or [`ActiveAnimFourGarrisoned`](/keys/activeanimfourgarrisoned/).
