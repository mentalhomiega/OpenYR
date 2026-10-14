---
key: SecondSpawnOffset
scope: aircrafttype
label: 'Second launch point'
see_also: [Spawns, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0,0,0"
---

The offset, in leptons, added to the firing point that a carried aircraft or missile launches from. It is added to the launch slot's firing point in the forward, lateral and height parts that [`PrimaryFireFLH`](/keys/primaryfireflh/) uses, and it turns with the object. A missile takes it on the odd-numbered launches when the object's first weapon has a [`Burst`](/keys/burst/) above `1`. Any other launch takes it only while the object's own burst index is not zero, which a burst weapon sets while it fires. A structure or infantry spawner ignores it. With the default `0,0,0` it changes nothing.

[Spawned aircraft and missiles](/systems/spawned-aircraft/#launching) gives the launch slot.

```ini title="rulesmd.ini"
[MYMISSILESHIP] ; example VesselType
SecondSpawnOffset=0,40,60
```
