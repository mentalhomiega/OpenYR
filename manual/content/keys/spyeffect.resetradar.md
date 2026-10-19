---
key: SpyEffect.ResetRadar
summary: With yes, infiltrating this structure shrouds the owner's map again.
see_also: ["SpyEffect.Custom", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyPercentage", "SpyEffect.StolenMoneyAmount", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), `SpyEffect.ResetRadar=yes` covers the owner's map with shroud again, leaving what the owner can see now, as the stock radar sabotage does. It has no effect on an owner that has a working spy satellite.
