---
key: Spotlight.Distance
scope: buildingtype
summary: "How far in front of a structure its spotlight lights the ground."
see_also: [HasSpotlight, Spotlight.StartHeight, SpotlightLocationRadius]
when_omitted:
  kind: computed
  note: "SpotlightLocationRadius from the General section"
---

On a [`HasSpotlight=yes`](/keys/hasspotlight/) structure, the pool of light lies this many leptons in front of the structure, in the direction it faces, and the beam sweeps about that point. A negative value uses [`SpotlightLocationRadius`](/keys/spotlightlocationradius/) instead.

```ini title="rulesmd.ini"
[MYLIGHT] ; example BuildingType
HasSpotlight=yes
Spotlight.Distance=2000
```
