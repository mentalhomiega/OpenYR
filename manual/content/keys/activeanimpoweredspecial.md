---
key: ActiveAnimPoweredSpecial
summary: A Yuri's Revenge power flag for active slot one that has no effect yet.
see_also: ["ActiveAnim", "ActiveAnimPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ActiveAnimPoweredSpecial=` is read for active slot one, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`ActiveAnim`](/keys/activeanim/), [`ActiveAnimDamaged`](/keys/activeanimdamaged/) or [`ActiveAnimGarrisoned`](/keys/activeanimgarrisoned/).
