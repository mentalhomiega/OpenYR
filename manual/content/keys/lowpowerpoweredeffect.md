---
key: LowPowerPoweredEffect
summary: Whether the low power slot's animation is removed during a power shortfall and restored when power returns.
see_also: ["LowPower", "LowPowerPowered", "LowPowerPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`LowPower`](/keys/lowpower/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`LowPowerPoweredLight`](/keys/lowpowerpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`LowPowerPowered`](/keys/lowpowerpowered/) and [`LowPowerPoweredLight`](/keys/lowpowerpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`LowPower`](/keys/lowpower/), [`LowPowerDamaged`](/keys/lowpowerdamaged/) or [`LowPowerGarrisoned`](/keys/lowpowergarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
