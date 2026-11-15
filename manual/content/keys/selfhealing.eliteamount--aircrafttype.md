---
key: SelfHealing.EliteAmount
scope: aircrafttype
summary: "The strength one self-healing step restores while this type is elite."
see_also: [SelfHealing.Amount, SelfHealing.Rate, SelfHealing.Max]
when_omitted:
  kind: inherited
  note: The SelfHealing.Amount, else SelfHealingStep, else the SelfHealStep of [General], else 1.
---

`SelfHealing.EliteAmount` is an Ares key. It is [`SelfHealing.Amount`](/keys/selfhealing.amount/) for an object that is elite. A value below zero counts as unset. A set `SelfHealing.Amount` takes its place.

```ini title="rulesmd.ini"
[APOC] ; example VehicleType
SelfHealing=yes
SelfHealing.RookieAmount=2
SelfHealing.VeteranAmount=4
SelfHealing.EliteAmount=6
```
