---
key: ActiveAnimFourPoweredSpecial
summary: A Yuri's Revenge power flag for active slot four that has no effect yet.
see_also: ["ActiveAnimFour", "ActiveAnimFourPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ActiveAnimFourPoweredSpecial=` is read for active slot four, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`ActiveAnimFour`](/keys/activeanimfour/), [`ActiveAnimFourDamaged`](/keys/activeanimfourdamaged/) or [`ActiveAnimFourGarrisoned`](/keys/activeanimfourgarrisoned/).
