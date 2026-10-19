---
key: Experience.PassengerModifier
scope: aircrafttype
summary: "Multiplies the experience this transport earns from its passengers' kills."
see_also: ["Experience.FromPassengers", "Experience.PromotePassengers", "Experience.FromAirstrike", "Experience.AirstrikeModifier", "Experience.MindControlSelfModifier", "Experience.MindControlVictimModifier", "Experience.SpawnOwnerModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "1"
---

A transport that earns experience for a kill its passenger makes gains the usual share multiplied by this number. A passenger credited itself, under [`Experience.PromotePassengers`](/keys/experience.promotepassengers/), gains the multiplied share too.

```ini title="rulesmd.ini"
[MYIFV] ; example VehicleType
Experience.PassengerModifier=150%
```
