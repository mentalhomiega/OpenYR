---
key: ChronoInSound
scope: global-rules
label: Default chronosphere arrival sound
see_also: [ChronoOutSound, WarpOut, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at each unit the [chronosphere](/systems/superweapons/#chronosphere) sets down, unless the unit's type names its own [`ChronoInSound`](/keys/chronoinsound/#scope-aircrafttype).

```ini title="rulesmd.ini"
[AudioVisual]
ChronoInSound=MyChronoIn ; a sound ID registered in SOUNDMD.INI
```
