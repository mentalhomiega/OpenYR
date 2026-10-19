---
key: Experience.FromPassengers
scope: aircrafttype
summary: "With no, a kill by a passenger does not promote this open-topped transport."
see_also: ["Experience.PromotePassengers", "Experience.PassengerModifier", "Experience.FromAirstrike", "Experience.AirstrikeModifier", "Experience.MindControlSelfModifier", "Experience.MindControlVictimModifier", "Experience.SpawnOwnerModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "yes"
---

A soldier that fires from an `OpenTopped=yes` transport earns the transport its experience when the transport is `Trainable=yes`. With `Experience.FromPassengers=no`, the transport earns none from its passengers' kills, and a `Trainable=yes` passenger is credited itself.

```ini title="rulesmd.ini"
[MYIFV] ; example VehicleType
Experience.FromPassengers=no
```
