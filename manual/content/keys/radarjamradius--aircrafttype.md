---
key: RadarJamRadius
scope: aircrafttype
summary: "The cells around an object within which it jams enemy radar structures."
see_also: [Radar, SpySat]
when_omitted:
  kind: value
  value: "0, so the object jams nothing"
---

With `RadarJamRadius=` above 0, every [`Radar=yes`](/keys/radar/) and [`SpySat=yes`](/keys/spysat/) structure within that many cells of the object is jammed while the object is on the map. Houses allied with the object's owner, the owner included, are not affected. A jammed radar structure stays on and keeps its other functions, but it gives its house no radar and no spy satellite map. A house with a working radar structure elsewhere keeps its radar. A jammer that moves is checked again about once a second.

```ini title="rulesmd.ini"
[WIDOW] ; example AircraftType
RadarJamRadius=16
```
