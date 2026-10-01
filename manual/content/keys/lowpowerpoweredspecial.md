---
key: LowPowerPoweredSpecial
summary: A Yuri's Revenge power flag for the low power slot that has no effect yet.
see_also: ["LowPower", "LowPowerPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`LowPowerPoweredSpecial=` is read for the low power slot, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/).
