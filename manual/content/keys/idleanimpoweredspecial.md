---
key: IdleAnimPoweredSpecial
summary: Marks the animation in the idle slot as one a power plant removes in a blackout or a drain.
see_also: ["IdleAnim", "IdleAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`IdleAnimPoweredSpecial=` is read for the idle slot, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`IdleAnim`](/keys/idleanim/), [`IdleAnimDamaged`](/keys/idleanimdamaged/) or [`IdleAnimGarrisoned`](/keys/idleanimgarrisoned/).
