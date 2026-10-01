---
key: SuperAnimPowered
summary: Whether super slot one's animation freezes while its house is short of power.
see_also: ["SuperAnim", "SuperAnimPoweredLight", "SuperAnimPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`SuperAnim`](/keys/superanim/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`SuperAnimPoweredLight`](/keys/superanimpoweredlight/) or [`SuperAnimPoweredEffect`](/keys/superanimpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`SuperAnim`](/keys/superanim/), [`SuperAnimDamaged`](/keys/superanimdamaged/) or [`SuperAnimGarrisoned`](/keys/superanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
