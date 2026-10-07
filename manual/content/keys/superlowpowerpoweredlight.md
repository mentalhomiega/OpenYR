---
key: SuperLowPowerPoweredLight
summary: Whether the super low power slot's animation is removed during a power shortfall and recreated each time the structure comes back into service.
see_also: ["SuperLowPower", "SuperLowPowerPowered", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SuperLowPower`](/keys/superlowpower/) animation is removed while its house is short of power. Each time the structure comes back into service, the animation is created again if the slot is empty, whether or not the shortfall emptied it. The flag works only beside [`SuperLowPowerPowered`](/keys/superlowpowerpowered/) set to `no`; with the default `yes` there the animation freezes instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
