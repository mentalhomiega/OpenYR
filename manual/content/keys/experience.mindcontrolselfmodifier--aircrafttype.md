---
key: Experience.MindControlSelfModifier
scope: aircrafttype
summary: "The share of a kill's experience a mind controller earns for the kills of its controlled objects."
see_also: ["Experience.FromPassengers", "Experience.PromotePassengers", "Experience.PassengerModifier", "Experience.FromAirstrike", "Experience.AirstrikeModifier", "Experience.MindControlVictimModifier", "Experience.SpawnOwnerModifier", "Experience.SpawnModifier", VeteranRatio, Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "0%"
---

Set on the controller. An object under its control that makes a kill earns the controlled object its usual experience, scaled by the controller's [`Experience.MindControlVictimModifier`](/keys/experience.mindcontrolvictimmodifier/), and the controller earns this number times the share the controlled object would have earned without that scaling, when it is `Trainable=yes`.

```ini title="rulesmd.ini"
[YURIX] ; example InfantryType
Experience.MindControlSelfModifier=50%
Experience.MindControlVictimModifier=50%  ; splits the experience between them
```
