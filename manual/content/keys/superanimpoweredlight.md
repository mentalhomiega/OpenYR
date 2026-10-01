---
key: SuperAnimPoweredLight
summary: Whether super slot one's animation is removed during a power shortfall and recreated at every full-power recheck.
see_also: ["SuperAnim", "SuperAnimPowered", "system:power"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the [`SuperAnim`](/keys/superanim/) animation is removed while its house is short of power. Each time the house rechecks its power at full power, the animation is created again if the slot is empty, whether or not the shortfall emptied it. The flag works only beside [`SuperAnimPowered`](/keys/superanimpowered/) set to `no`; with the default `yes` there the animation freezes instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power is affected. The value is read only when the slot has an animation name from [`SuperAnim`](/keys/superanim/), [`SuperAnimDamaged`](/keys/superanimdamaged/) or [`SuperAnimGarrisoned`](/keys/superanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
