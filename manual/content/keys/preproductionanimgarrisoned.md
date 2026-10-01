---
key: PreProductionAnimGarrisoned
summary: The animation the pre-production slot runs while infantry occupy the structure.
see_also: ["PreProductionAnim", "PreProductionAnimDamaged"]
when_omitted:
  kind: inherited
  note: The animation PreProductionAnim names.
---

`PreProductionAnimGarrisoned=` names the animation the pre-production slot is meant to run in place of [`PreProductionAnim`](/keys/preproductionanim/) while infantry occupy the structure. No structure can be occupied yet, so the value is read but never used. [The damaged form](/systems/building-animations/#the-damaged-form) covers the slot's three names.
