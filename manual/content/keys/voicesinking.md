---
key: VoiceSinking
summary: "The voice an object plays as it starts to sink."
see_also: [SinkingSound]
when_omitted:
  kind: value
  value: none
---

An object plays this voice at its position as it starts to sink, together with its [`SinkingSound`](/keys/sinkingsound/#scope-aircrafttype).

```ini title="rulesmd.ini"
[MYBOAT] ; example VehicleType
VoiceSinking=MYBOAT_Sinking
```
