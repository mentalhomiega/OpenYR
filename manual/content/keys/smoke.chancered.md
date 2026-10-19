---
key: Smoke.ChanceRed
summary: The odds in percent each frame that a badly damaged aircraft of this type creates a smoke animation.
see_also: ["Smoke.Anim", "Smoke.ChanceDead", ConditionRed]
when_omitted:
  kind: value
  value: "10"
---

`Smoke.ChanceRed` is a whole number of percent. Each frame that an aircraft of this type is in the air with its health below [`ConditionRed`](/keys/conditionred/) and above 0, it makes a [`Smoke.Anim`](/keys/smoke.anim/) with this chance. `0` creates none.
