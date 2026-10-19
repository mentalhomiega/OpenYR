---
key: Experience.SpawnOwnerModifier
scope: aircrafttype
summary: "The share of a spawned craft's kill experience its spawner earns."
see_also: ["Experience.FromPassengers", "Experience.PromotePassengers", "Experience.PassengerModifier", "Experience.FromAirstrike", "Experience.AirstrikeModifier", "Experience.MindControlSelfModifier", "Experience.MindControlVictimModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "0%"
---

Set on the spawner. When a craft it launched makes a kill, the spawner earns this number times its usual share for the kill, counted from the spawner's own cost, when it is `Trainable=yes`. The craft earns its own experience as well. A missile spawn is not covered: its launcher earns all of its experience as stock.

```ini title="rulesmd.ini"
[CARRIER] ; example VehicleType
Experience.SpawnOwnerModifier=100%
```
