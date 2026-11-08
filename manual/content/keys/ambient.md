---
key: Ambient
summary: The scenario's overall light level, where 1 is full daylight.
see_also: [IonAmbient, AmbientChangeRate, AmbientChangeStep, "system:ion-storms"]
when_omitted:
  kind: value
  value: "1"
---

```ini title="map file"
[Lighting]
Ambient=1
```

`1` is full daylight, `.5` is half as bright, and a value above `1`, such as `1.35`, is brighter than daylight. The level is kept in hundredths. The map starts at this level as it loads, with no fade.

[`IonAmbient`](/keys/ionambient/) does not fall back to this value: an omitted `IonAmbient` is `.87`, so a storm on a map that sets only `Ambient` fades to `.87`. When [a storm ends](/systems/ion-storms/#the-storm-ends), the map fades back to this level through [the ambient fade](/systems/ion-storms/#the-ambient-ramp).

The [Set ambient light...](/mapping/actions/taction-set-ambient-light/) trigger action replaces this level during play, and the map fades to the new level. During an ion storm the new level is kept, and the fade to it starts when the storm ends.
