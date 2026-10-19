---
key: Experience.FromAirstrike
scope: aircrafttype
summary: "With yes, the object that called an air strike earns the experience of its planes' kills."
see_also: ["Experience.FromPassengers", "Experience.PromotePassengers", "Experience.PassengerModifier", "Experience.AirstrikeModifier", "Experience.MindControlSelfModifier", "Experience.MindControlVictimModifier", "Experience.SpawnOwnerModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "no"
---

A kill that a plane of an object's air strike makes earns the object that called the strike the experience, when it is `Trainable=yes`. The plane earns none.

```ini title="rulesmd.ini"
[MYDESIGNATOR] ; example InfantryType
Experience.FromAirstrike=yes
```
