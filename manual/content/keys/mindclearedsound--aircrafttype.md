---
key: MindClearedSound
scope: aircrafttype
label: Mind control release sound
see_also: [YuriMindControlSound, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

Plays where an object of this type is [let go](/systems/mind-control/#letting-go) by mind control. A type that names no sound uses [`MindClearedSound`](/keys/mindclearedsound/#scope-global-rules) from `[AudioVisual]`.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
MindClearedSound=MyTankFreed ; a sound ID registered in SOUNDMD.INI
```
