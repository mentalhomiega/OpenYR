---
key: SuperAnimTwoPowered
summary: Whether super slot two's animation freezes while its house is short of power.
see_also: ["SuperAnimTwo", "SuperAnimTwoPoweredLight", "SuperAnimTwoPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`SuperAnimTwo`](/keys/superanimtwo/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`SuperAnimTwoPoweredLight`](/keys/superanimtwopoweredlight/) or [`SuperAnimTwoPoweredEffect`](/keys/superanimtwopoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`SuperAnimTwo`](/keys/superanimtwo/), [`SuperAnimTwoDamaged`](/keys/superanimtwodamaged/) or [`SuperAnimTwoGarrisoned`](/keys/superanimtwogarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
