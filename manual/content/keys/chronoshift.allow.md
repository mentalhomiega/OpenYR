---
key: Chronoshift.Allow
summary: Lets the chronosphere skip a vehicle, infantry type or landed aircraft, leaving it where it is.
see_also: [Organic, Teleporter, "Chronoshift.Crushable", "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `Chronoshift.Allow=no`, the [chronosphere](/systems/superweapons/#chronosphere) leaves an object where it is when it stands in the picked area. It is neither moved nor destroyed, including an [`Organic=yes`](/keys/organic/) infantryman that is not [`Teleporter=yes`](/keys/teleporter/), which the chronosphere would otherwise destroy.

The key affects only vehicles, infantry and landed aircraft, the objects the chronosphere handles. A structure is never moved, so the key does nothing on a BuildingType.

A skipped object can still be destroyed if another unit lands on it. Set [`Chronoshift.Crushable=no`](/keys/chronoshift.crushable/) to prevent that.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Chronoshift.Allow=no
```
