---
key: SpyEffect.VehicleVeterancy
summary: With yes, infiltrating this structure makes the infiltrator's new trainable land vehicles veterans.
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyPercentage", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), every land vehicle of a [`Trainable=yes`](/keys/trainable/) type that the infiltrating house gets from then on starts as a veteran. Ships are not included; see [`SpyEffect.NavalVeterancy`](/keys/spyeffect.navalveterancy/). `SpyEffect.UnitVeterancy` is read as the same key.
