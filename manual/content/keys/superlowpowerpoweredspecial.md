---
key: SuperLowPowerPoweredSpecial
summary: A Yuri's Revenge power flag for the super low power slot that has no effect yet.
see_also: ["SuperLowPower", "SuperLowPowerPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SuperLowPowerPoweredSpecial=` is read for the super low power slot, but nothing uses it yet. In Yuri's Revenge it marks an animation that a `PoweredSpecial=yes` structure removes while its house is blacked out. The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/).
