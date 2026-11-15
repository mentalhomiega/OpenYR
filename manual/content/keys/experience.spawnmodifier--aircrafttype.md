---
key: Experience.SpawnModifier
scope: aircrafttype
summary: "Multiplies the experience a craft this spawner launched earns for its kills."
see_also: ["Experience.FromPassengers", "Experience.PromotePassengers", "Experience.PassengerModifier", "Experience.FromAirstrike", "Experience.AirstrikeModifier", "Experience.MindControlSelfModifier", "Experience.MindControlVictimModifier", "Experience.SpawnOwnerModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "100%"
---

Set on the spawner. A `Trainable=yes` craft it launched earns this number times the usual share for a kill.

```ini title="rulesmd.ini"
[CARRIER] ; example VehicleType
Experience.SpawnModifier=50%
```
