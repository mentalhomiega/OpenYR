---
key: SpyEffect.StolenMoneyPercentage
summary: The share of the owner's credits a spy steals from this structure.
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyAmount", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "0"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), the infiltrating house takes this share of the owner's credits, as a fraction or with a percent sign, and the owner loses it. With [`SpyEffect.StolenMoneyAmount`](/keys/spyeffect.stolenmoneyamount/) also set, the smaller of the two amounts is taken. The theft never exceeds the owner's credits.
