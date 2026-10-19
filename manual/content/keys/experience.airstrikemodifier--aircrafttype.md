---
key: Experience.AirstrikeModifier
scope: aircrafttype
summary: "Multiplies the experience the caller of an air strike earns from its planes' kills."
see_also: ["Experience.FromPassengers", "Experience.PromotePassengers", "Experience.PassengerModifier", "Experience.FromAirstrike", "Experience.MindControlSelfModifier", "Experience.MindControlVictimModifier", "Experience.SpawnOwnerModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "1"
---

The number multiplies the usual share, which is counted from the caller's own cost. The key has an effect only with [`Experience.FromAirstrike=yes`](/keys/experience.fromairstrike/).

```ini title="rulesmd.ini"
[MYDESIGNATOR] ; example InfantryType
Experience.FromAirstrike=yes
Experience.AirstrikeModifier=200%
```
