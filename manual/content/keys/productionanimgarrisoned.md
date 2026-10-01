---
key: ProductionAnimGarrisoned
summary: The animation the production slot runs while infantry occupy the structure.
see_also: ["ProductionAnim", "ProductionAnimDamaged"]
when_omitted:
  kind: inherited
  note: The animation ProductionAnim names.
---

`ProductionAnimGarrisoned=` names the animation the production slot is meant to run in place of [`ProductionAnim`](/keys/productionanim/) while infantry occupy the structure. No structure can be occupied yet, so the value is read but never used. [The damaged form](/systems/building-animations/#the-damaged-form) covers the slot's three names.
