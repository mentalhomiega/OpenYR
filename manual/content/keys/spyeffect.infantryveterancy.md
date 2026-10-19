---
key: SpyEffect.InfantryVeterancy
summary: With yes, infiltrating this structure makes the infiltrator's new trainable soldiers veterans.
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyPercentage", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), every soldier of a [`Trainable=yes`](/keys/trainable/) type that the infiltrating house gets from then on starts as a veteran. It is the effect of infiltrating a stock barracks.
