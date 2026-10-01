---
key: SpecialAnimFourPoweredLight
summary: Whether special slot four's animation is removed during a power shortfall and recreated at every full-power recheck.
see_also: ["SpecialAnimFour", "SpecialAnimFourPowered", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SpecialAnimFour`](/keys/specialanimfour/) animation is removed while its house is short of power. Each time the house rechecks its power at full power, the animation is created again if the slot is empty, whether or not the shortfall emptied it. The flag works only beside [`SpecialAnimFourPowered`](/keys/specialanimfourpowered/) set to `no`; with the default `yes` there the animation freezes instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SpecialAnimFour`](/keys/specialanimfour/), [`SpecialAnimFourDamaged`](/keys/specialanimfourdamaged/) or [`SpecialAnimFourGarrisoned`](/keys/specialanimfourgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
