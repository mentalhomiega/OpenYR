---
key: SolidHeight
summary: "How many cells of a building's height stop a SubjectToBuildings projectile."
see_also: [SubjectToBuildings, AlliedSolidTransparency]
when_omitted:
  kind: value
  value: "0, so the building is not solid"
---

A [`SubjectToBuildings=yes`](/keys/subjecttobuildings/) projectile is stopped by this building while it is lower than `SolidHeight` cells of 256 leptons above the ground. A negative value makes the building's whole height solid, which is the `Height` its art section gives in terrain levels. At `0` projectiles ignore the building.

```ini title="rulesmd.ini"
[GAPOWR] ; example BuildingType
SolidHeight=-1
```
