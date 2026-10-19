---
key: Smoke.ChanceDead
summary: The odds in percent each frame that a crashing aircraft of this type creates a smoke animation.
see_also: ["Smoke.Anim", "Smoke.ChanceRed", ConditionRed]
when_omitted:
  kind: value
  value: "80"
---

`Smoke.ChanceDead` is a whole number of percent. Each frame that an aircraft of this type is in the air with no health left, which is while it crashes, it makes a [`Smoke.Anim`](/keys/smoke.anim/) with this chance. `0` creates none.
