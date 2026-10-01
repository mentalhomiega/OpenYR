---
key: ImpactWaterSound
scope: global-rules
label: Default crash sound on water
see_also: [Explodes]
when_omitted:
  kind: value
  value: none
---

When a destroyed aircraft whose type sets no [`ImpactWaterSound`](/keys/impactwatersound/#scope-aircrafttype) of its own falls and hits water, this sound plays there.

```ini title="rulesmd.ini"
[AudioVisual]
ImpactWaterSound=ExplosionWaterLarge
```
