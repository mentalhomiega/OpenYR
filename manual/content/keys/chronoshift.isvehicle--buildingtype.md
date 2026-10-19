---
key: Chronoshift.IsVehicle
scope: buildingtype
summary: "Marks a structure as a deployed vehicle for the chronosphere."
see_also: ["Chronosphere.ReconsiderBuildings", "Chronosphere.AffectsUndeployable", "Chronoshift.Allow", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a chronosphere whose [`Chronosphere.ReconsiderBuildings=`](/keys/chronosphere.reconsiderbuildings/) is on treats the structure as a vehicle: it is moved when [`SW.AffectsTarget=`](/keys/sw.affectstarget/) allows units, ignored when the weapon moves only structures, and it lands on the nearest free foundation instead of failing when its exact place is taken.

```ini title="rulesmd.ini"
[NAMOBL] ; example BuildingType
Chronoshift.IsVehicle=yes
```

See [Chronosphere](/systems/superweapons/#chronosphere).
