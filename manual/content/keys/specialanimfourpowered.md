---
key: SpecialAnimFourPowered
summary: Whether special slot four's animation freezes while its house is short of power.
see_also: ["SpecialAnimFour", "SpecialAnimFourPoweredLight", "SpecialAnimFourPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`SpecialAnimFour`](/keys/specialanimfour/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`SpecialAnimFourPoweredLight`](/keys/specialanimfourpoweredlight/) or [`SpecialAnimFourPoweredEffect`](/keys/specialanimfourpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`SpecialAnimFour`](/keys/specialanimfour/), [`SpecialAnimFourDamaged`](/keys/specialanimfourdamaged/) or [`SpecialAnimFourGarrisoned`](/keys/specialanimfourgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
