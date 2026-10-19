---
key: Experience.MindControlVictimModifier
scope: aircrafttype
summary: "Multiplies the experience an object earns while this mind controller controls it."
see_also: ["Experience.FromPassengers", "Experience.PromotePassengers", "Experience.PassengerModifier", "Experience.FromAirstrike", "Experience.AirstrikeModifier", "Experience.MindControlSelfModifier", "Experience.SpawnOwnerModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "100%"
---

Set on the controller. The kills of an object under its control earn that object this number times the usual share.

```ini title="rulesmd.ini"
[YURIX] ; example InfantryType
Experience.MindControlVictimModifier=50%
```
