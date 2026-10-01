---
key: SelfHealUnitFrames
summary: How often machine-shop-type structures mend their owner's vehicles, in frames.
see_also: [SelfHealUnitAmount, UnitsGainSelfHeal, "system:repair"]
when_omitted:
  kind: value
  value: "1000"
---

Vehicles owned by a house with [`UnitsGainSelfHeal`](/keys/unitsgainselfheal/) structures heal on every frame that is a multiple of this value.

```ini title="rulesmd.ini"
[General]
SelfHealUnitFrames=1000
```

A value of `0` or below turns this healing off.
