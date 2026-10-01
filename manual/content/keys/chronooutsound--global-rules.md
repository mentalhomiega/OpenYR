---
key: ChronoOutSound
scope: global-rules
label: Default chronosphere departure sound
see_also: [ChronoInSound, WarpOut, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at each unit the [chronosphere](/systems/superweapons/#chronosphere) picks up, just before it leaves, unless the unit's type names its own [`ChronoOutSound`](/keys/chronooutsound/#scope-aircrafttype).

```ini title="rulesmd.ini"
[AudioVisual]
ChronoOutSound=MyChronoOut ; a sound ID registered in SOUNDMD.INI
```
