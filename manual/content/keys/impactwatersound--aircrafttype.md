---
key: ImpactWaterSound
scope: aircrafttype
label: Crash sound on water
see_also: [Explodes]
when_omitted:
  kind: value
  value: none
---

When a destroyed aircraft or jumpjet of this type falls and hits water, this sound plays there. Without it, the rules' [`ImpactWaterSound`](/keys/impactwatersound/#scope-global-rules) plays.

```ini title="rulesmd.ini"
[MYJET] ; example AircraftType
ImpactWaterSound=MyCrashSound
```
