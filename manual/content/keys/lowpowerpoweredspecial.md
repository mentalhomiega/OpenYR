---
key: LowPowerPoweredSpecial
summary: Marks the animation in the low power slot as one a power plant removes in a blackout or a drain.
see_also: ["LowPower", "LowPowerPoweredEffect"]
when_omitted:
  kind: value
  value: "no"
---

`LowPowerPoweredSpecial=` is read for the low power slot, and marks an animation that a [`PoweredSpecial=yes`](/keys/poweredspecial/) structure removes while it is out of service in a blackout or being drained. The structure plays its `LowPower` animation instead, and the marked animation starts again when the structure works again. The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/).
