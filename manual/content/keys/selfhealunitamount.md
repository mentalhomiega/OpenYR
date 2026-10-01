---
key: SelfHealUnitAmount
summary: The strength machine-shop-type structures restore to each damaged vehicle, per point of UnitsGainSelfHeal.
see_also: [SelfHealUnitFrames, UnitsGainSelfHeal, "system:repair"]
when_omitted:
  kind: value
  value: "1"
---

Each time [`SelfHealUnitFrames`](/keys/selfhealunitframes/) comes round, a damaged vehicle gains this value times the total [`UnitsGainSelfHeal`](/keys/unitsgainselfheal/) of its house's structures, up to its maximum strength.

```ini title="rulesmd.ini"
[General]
SelfHealUnitAmount=1
```

A value of `0` or below turns this healing off.
