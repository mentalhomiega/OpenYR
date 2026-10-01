---
key: ActiveAnimTwoPoweredSpecial
summary: A Yuri's Revenge power flag for active slot two that has no effect yet.
see_also: ["ActiveAnimTwo", "ActiveAnimTwoPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`ActiveAnimTwoPoweredSpecial=` is read for active slot two, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`ActiveAnimTwo`](/keys/activeanimtwo/), [`ActiveAnimTwoDamaged`](/keys/activeanimtwodamaged/) or [`ActiveAnimTwoGarrisoned`](/keys/activeanimtwogarrisoned/).
