---
key: SpyEffect.StolenMoneyAmount
summary: The number of credits a spy steals from this structure.
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyPercentage", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "0"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), the infiltrating house takes this many credits from the owner, up to what the owner has. With [`SpyEffect.StolenMoneyPercentage`](/keys/spyeffect.stolenmoneypercentage/) also set, the smaller of the two amounts is taken.
