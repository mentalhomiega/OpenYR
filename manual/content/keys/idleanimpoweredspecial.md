---
key: IdleAnimPoweredSpecial
summary: A Yuri's Revenge power flag for the idle slot that has no effect yet.
see_also: ["IdleAnim", "IdleAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`IdleAnimPoweredSpecial=` is read for the idle slot, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`IdleAnim`](/keys/idleanim/), [`IdleAnimDamaged`](/keys/idleanimdamaged/) or [`IdleAnimGarrisoned`](/keys/idleanimgarrisoned/).
