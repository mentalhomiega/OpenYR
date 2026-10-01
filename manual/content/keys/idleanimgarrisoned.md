---
key: IdleAnimGarrisoned
summary: The animation the idle slot runs while infantry occupy the structure.
see_also: ["IdleAnim", "IdleAnimDamaged"]
when_omitted:
  kind: inherited
  note: The animation IdleAnim names.
---

`IdleAnimGarrisoned=` names the animation the idle slot is meant to run in place of [`IdleAnim`](/keys/idleanim/) while infantry occupy the structure. No structure can be occupied yet, so the value is read but never used. [The damaged form](/systems/building-animations/#the-damaged-form) covers the slot's three names.
