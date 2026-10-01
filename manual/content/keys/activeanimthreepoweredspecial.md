---
key: ActiveAnimThreePoweredSpecial
summary: A Yuri's Revenge power flag for active slot three that has no effect yet.
see_also: ["ActiveAnimThree", "ActiveAnimThreePoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ActiveAnimThreePoweredSpecial=` is read for active slot three, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`ActiveAnimThree`](/keys/activeanimthree/), [`ActiveAnimThreeDamaged`](/keys/activeanimthreedamaged/) or [`ActiveAnimThreeGarrisoned`](/keys/activeanimthreegarrisoned/).
