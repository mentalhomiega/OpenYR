---
key: SpyEffect.PowerOutageDuration
summary: How many frames infiltrating this structure cuts the owner's power.
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", "SpyEffect.StolenMoneyPercentage", "SpyEffect.StolenMoneyAmount", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "0"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), a value above `0` starts a power outage of that many frames for the owner, which restarts one already running. The owner's structures make no power while it lasts. `0` leaves power alone, and it is the default.
