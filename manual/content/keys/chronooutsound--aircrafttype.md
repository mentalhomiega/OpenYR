---
key: ChronoOutSound
scope: aircrafttype
label: Chronosphere departure sound
see_also: [ChronoInSound, WarpOut, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays where the [chronosphere](/systems/superweapons/#chronosphere) picks a unit of this type up, just before it leaves. A type that names no sound uses [`ChronoOutSound`](/keys/chronooutsound/#scope-global-rules) from `[AudioVisual]`.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
ChronoOutSound=MyTankChronoOut ; a sound ID registered in SOUNDMD.INI
```
