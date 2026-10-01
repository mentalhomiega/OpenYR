---
key: ImpactLandSound
scope: aircrafttype
label: Crash sound on the ground
see_also: [Explodes]
when_omitted:
  kind: value
  value: none
---

When a destroyed aircraft or jumpjet of this type falls and hits the ground, this sound plays there. Without it, the rules' [`ImpactLandSound`](/keys/impactlandsound/#scope-global-rules) plays.

```ini title="rulesmd.ini"
[MYJET] ; example AircraftType
ImpactLandSound=MyCrashSound
```
