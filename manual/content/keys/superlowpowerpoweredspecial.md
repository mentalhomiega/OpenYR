---
key: SuperLowPowerPoweredSpecial
summary: Marks the animation in the super low power slot as one a power plant removes in a blackout or a drain.
see_also: ["SuperLowPower", "SuperLowPowerPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`SuperLowPowerPoweredSpecial=` is read for the super low power slot, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/).
