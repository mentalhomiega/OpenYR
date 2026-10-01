---
key: ImpactLandSound
scope: global-rules
label: Default crash sound on the ground
see_also: [Explodes]
when_omitted:
  kind: value
  value: none
---

When a destroyed aircraft whose type sets no [`ImpactLandSound`](/keys/impactlandsound/#scope-aircrafttype) of its own falls and hits the ground, this sound plays there.

```ini title="rulesmd.ini"
[AudioVisual]
ImpactLandSound=ExplosionWaterLarge
```
