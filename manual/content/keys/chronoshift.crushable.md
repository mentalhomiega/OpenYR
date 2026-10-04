---
key: Chronoshift.Crushable
summary: Stops a unit the chronosphere sets down on this object from destroying it; the arriving unit is destroyed instead.
see_also: [ChronoInfantryCrush, "Chronoshift.Allow", "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `Chronoshift.Crushable=no`, a unit the [chronosphere](/systems/superweapons/#chronosphere) sets down on this object's spot is destroyed, and this object is untouched. By default the arriving unit destroys the object.

Only a vehicle, infantryman or aircraft on the landing cell can be crushed, so the key has no effect on a BuildingType. It applies only when the landing cell has no structure or terrain object such as a tree. If one is there, the arriving unit moves to the nearest cell it can stand on and this key is not read.

One object with `Chronoshift.Crushable=no` in the way is enough to destroy the arriving unit, and the other objects it would have crushed are left alone too. An arriving infantryman reaches only infantry on its own spot, but any vehicle or aircraft in the cell. The [chronosphere](/systems/superweapons/#chronosphere) page lists what else destroys the arriving unit.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Chronoshift.Crushable=no
```
