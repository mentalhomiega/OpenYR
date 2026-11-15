---
key: Spotlight.StartHeight
scope: buildingtype
summary: "How high above the ground a structure's spotlight beam starts."
see_also: [HasSpotlight, Spotlight.Distance]
when_omitted:
  kind: value
  value: "430 leptons"
---

On a [`HasSpotlight=yes`](/keys/hasspotlight/) structure, the beam is drawn from a point this many leptons above the structure's center, and its two glow lines run from there to the pool of light on the ground. A negative value reads as 0.

```ini title="rulesmd.ini"
[MYLIGHT] ; example BuildingType
HasSpotlight=yes
Spotlight.StartHeight=600
```
