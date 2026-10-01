---
key: SuperAnimFourPowered
summary: Whether super slot four's animation freezes while its house is short of power.
see_also: ["SuperAnimFour", "SuperAnimFourPoweredLight", "SuperAnimFourPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`SuperAnimFour`](/keys/superanimfour/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`SuperAnimFourPoweredLight`](/keys/superanimfourpoweredlight/) or [`SuperAnimFourPoweredEffect`](/keys/superanimfourpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`SuperAnimFour`](/keys/superanimfour/), [`SuperAnimFourDamaged`](/keys/superanimfourdamaged/) or [`SuperAnimFourGarrisoned`](/keys/superanimfourgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
