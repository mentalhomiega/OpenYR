---
key: ChronoInSound
scope: aircrafttype
label: Chronosphere arrival sound
see_also: [ChronoOutSound, WarpOut, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays where the [chronosphere](/systems/superweapons/#chronosphere) sets a unit of this type down. A type that names no sound uses [`ChronoInSound`](/keys/chronoinsound/#scope-global-rules) from `[AudioVisual]`.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
ChronoInSound=MyTankChronoIn ; a sound ID registered in SOUNDMD.INI
```
