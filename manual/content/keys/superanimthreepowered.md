---
key: SuperAnimThreePowered
summary: Whether super slot three's animation freezes while its house is short of power.
see_also: ["SuperAnimThree", "SuperAnimThreePoweredLight", "SuperAnimThreePoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`SuperAnimThree`](/keys/superanimthree/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`SuperAnimThreePoweredLight`](/keys/superanimthreepoweredlight/) or [`SuperAnimThreePoweredEffect`](/keys/superanimthreepoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`SuperAnimThree`](/keys/superanimthree/), [`SuperAnimThreeDamaged`](/keys/superanimthreedamaged/) or [`SuperAnimThreeGarrisoned`](/keys/superanimthreegarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
