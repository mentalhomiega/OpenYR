---
key: IdleAnimPowered
summary: Whether the idle slot's animation freezes while its house is short of power.
see_also: ["IdleAnim", "IdleAnimPoweredLight", "IdleAnimPoweredEffect", "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, the [`IdleAnim`](/keys/idleanim/) animation freezes on its current frame while its house is short of power, and resumes when the house has full power again. With `no`, a shortfall does not freeze it, and [`IdleAnimPoweredLight`](/keys/idleanimpoweredlight/) or [`IdleAnimPoweredEffect`](/keys/idleanimpoweredeffect/) can decide what happens instead. Only a [`Powered=yes`](/keys/powered/) structure that drains power reacts to a shortfall. The value is read only when the slot has an animation name from [`IdleAnim`](/keys/idleanim/), [`IdleAnimDamaged`](/keys/idleanimdamaged/) or [`IdleAnimGarrisoned`](/keys/idleanimgarrisoned/). [Power](/systems/building-animations/#power) covers the four power flags.
