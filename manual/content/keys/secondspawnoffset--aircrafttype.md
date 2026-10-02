---
key: SecondSpawnOffset
scope: aircrafttype
label: 'Second launch point'
see_also: [Spawns, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0,0,0"
---

The point, in leptons from the object's center, that every second carried aircraft or missile leaves from. The others leave from the object's primary weapon firing point. With the default `0,0,0` all of them use the firing point.

```ini title="rulesmd.ini"
[MYMISSILESHIP] ; example VesselType
SecondSpawnOffset=0,40,60
```
