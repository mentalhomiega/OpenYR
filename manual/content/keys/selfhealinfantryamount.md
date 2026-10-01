---
key: SelfHealInfantryAmount
summary: The strength hospital-type structures restore to each damaged infantryman, per point of InfantryGainSelfHeal.
see_also: [SelfHealInfantryFrames, InfantryGainSelfHeal, "system:repair"]
when_omitted:
  kind: value
  value: "1"
---

Each time [`SelfHealInfantryFrames`](/keys/selfhealinfantryframes/) comes round, a damaged infantryman gains this value times the total [`InfantryGainSelfHeal`](/keys/infantrygainselfheal/) of its house's structures, up to its maximum strength.

```ini title="rulesmd.ini"
[General]
SelfHealInfantryAmount=1
```

A value of `0` or below turns this healing off.
