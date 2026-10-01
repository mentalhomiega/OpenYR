---
key: SuperLowPowerPowered
summary: Whether the super low power slot's animation freezes while its house is short of power.
see_also: ["SuperLowPower", "SuperLowPowerPoweredLight", "SuperLowPowerPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`SuperLowPower`](/keys/superlowpower/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`SuperLowPowerPoweredLight`](/keys/superlowpowerpoweredlight/) or [`SuperLowPowerPoweredEffect`](/keys/superlowpowerpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`SuperLowPower`](/keys/superlowpower/), [`SuperLowPowerDamaged`](/keys/superlowpowerdamaged/) or [`SuperLowPowerGarrisoned`](/keys/superlowpowergarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
