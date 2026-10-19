---
key: SpyEffect.NavalVeterancy
summary: With yes, infiltrating this structure makes the infiltrator's new trainable ships veterans.
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyPercentage", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), every ship of a [`Trainable=yes`](/keys/trainable/) vehicle type with [`Naval=yes`](/keys/naval/) that the infiltrating house gets from then on starts as a veteran.
