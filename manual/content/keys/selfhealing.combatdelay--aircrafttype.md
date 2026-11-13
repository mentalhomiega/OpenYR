---
key: SelfHealing.CombatDelay
scope: aircrafttype
summary: "The frames after a damaging hit during which this type does not heal itself."
see_also: [SelfHealing, SelfHealing.Rate, SelfHealing.Amount]
when_omitted:
  kind: value
  value: "0"
---

`SelfHealing.CombatDelay` is an Ares key. Each hit that damages the object restarts a countdown of this many frames, and the object does not self-heal until it ends. Healing, such as a repair or a healing weapon, does not start it. `0` turns the delay off, as stock Yuri's Revenge has it. A negative value counts as `0`.

```ini title="rulesmd.ini"
[FV] ; example VehicleType
SelfHealing=yes
SelfHealing.CombatDelay=100
```
