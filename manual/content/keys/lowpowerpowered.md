---
key: LowPowerPowered
summary: Whether the low power slot's animation freezes while its house is short of power.
see_also: ["LowPower", "LowPowerPoweredLight", "LowPowerPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`LowPower`](/keys/lowpower/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`LowPowerPoweredLight`](/keys/lowpowerpoweredlight/) or [`LowPowerPoweredEffect`](/keys/lowpowerpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
