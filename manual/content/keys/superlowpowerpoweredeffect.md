---
key: SuperLowPowerPoweredEffect
summary: Whether the super low power slot's animation is removed during a power shortfall and restored when power returns.
see_also: ["SuperLowPower", "SuperLowPowerPowered", "SuperLowPowerPoweredLight", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SuperLowPower`](/keys/superlowpower/) animation is removed while its house is short of power, and created again when the house next rechecks its power at full power. Unlike [`SuperLowPowerPoweredLight`](/keys/superlowpowerpoweredlight/), it returns only if the shortfall removed it. The flag works only when [`SuperLowPowerPowered`](/keys/superlowpowerpowered/) and [`SuperLowPowerPoweredLight`](/keys/superlowpowerpoweredlight/) are both `no`. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
