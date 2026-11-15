---
key: SelfHealing.Rate
scope: aircrafttype
summary: "The minutes between the self-healing steps of this type."
see_also: [SelfHealingRate, SelfHealing, RepairRate]
when_omitted:
  kind: inherited
  note: The rate that SelfHealingRate, SelfHealRate or RepairRate gives.
---

`SelfHealing.Rate` is another spelling of [`SelfHealingRate`](/keys/selfhealingrate/), and the one written last wins.

```ini title="rulesmd.ini"
[ASSN] ; example InfantryType
SelfHealing=yes
SelfHealing.Rate=0.01
```
