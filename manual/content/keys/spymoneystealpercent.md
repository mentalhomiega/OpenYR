---
key: SpyMoneyStealPercent
summary: "The share of a house's money a spy steals from its refinery or silo."
see_also: [SpyPowerBlackout, Storage, "system:capture"]
when_omitted:
  kind: value
  value: "0.5"
---

When a spy walks into another house's structure with positive [`Storage`](/keys/storage/), the spy's house takes this share of that house's money, rounded down. [Infiltrating it](/systems/capture/#infiltrating-it) lists the other spy effects.

```ini title="rulesmd.ini"
[General]
SpyMoneyStealPercent=0.5
```

The money taken counts stored ore as well as loose credits, as any spending does.
