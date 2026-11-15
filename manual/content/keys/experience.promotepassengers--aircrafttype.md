---
key: Experience.PromotePassengers
scope: aircrafttype
summary: "With yes, an elite transport passes the experience of a passenger's kill to the passenger."
see_also: ["Experience.FromPassengers", "Experience.PassengerModifier", "Experience.FromAirstrike", "Experience.AirstrikeModifier", "Experience.MindControlSelfModifier", "Experience.MindControlVictimModifier", "Experience.SpawnOwnerModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "no"
---

While an `OpenTopped=yes` transport is already elite, the experience of a kill its passenger makes goes to the passenger, when that soldier is `Trainable=yes`. Without the key the transport keeps the experience, which an elite transport cannot use.

```ini title="rulesmd.ini"
[MYIFV] ; example VehicleType
Experience.PromotePassengers=yes
```
