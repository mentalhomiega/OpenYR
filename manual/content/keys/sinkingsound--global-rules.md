---
key: SinkingSound
scope: global-rules
label: Default sinking sound
see_also: [ShipSinkingWeight]
when_omitted:
  kind: value
  value: none
---

An object whose type sets no [`SinkingSound`](/keys/sinkingsound/#scope-aircrafttype) plays this sound at its position as it starts to sink.

```ini title="rulesmd.ini"
[AudioVisual]
SinkingSound=GenLargeWaterDie
```
