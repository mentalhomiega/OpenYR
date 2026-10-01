---
key: SelfHealInfantryFrames
summary: How often hospital-type structures mend their owner's infantry, in frames.
see_also: [SelfHealInfantryAmount, InfantryGainSelfHeal, "system:repair"]
when_omitted:
  kind: value
  value: "1000"
---

Infantry owned by a house with [`InfantryGainSelfHeal`](/keys/infantrygainselfheal/) structures heals on every frame that is a multiple of this value.

```ini title="rulesmd.ini"
[General]
SelfHealInfantryFrames=1000
```

A value of `0` or below turns this healing off.
