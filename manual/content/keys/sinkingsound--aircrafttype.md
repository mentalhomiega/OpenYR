---
key: SinkingSound
scope: aircrafttype
label: Object sinking sound
see_also: [ShipSinkingWeight, VoiceSinking]
when_omitted:
  kind: value
  value: none
  note: The object plays the `[AudioVisual]` `SinkingSound` instead.
---

An object plays this sound at its position as it starts to sink, whether a [destroyed ship](/keys/shipsinkingweight/) going down or a vehicle falling through broken ice.

```ini title="rulesmd.ini"
[DEST] ; Destroyer
SinkingSound=GenLargeWaterDie
```
