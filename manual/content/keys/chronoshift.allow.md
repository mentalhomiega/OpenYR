---
key: Chronoshift.Allow
summary: Lets the chronosphere skip a vehicle, infantry type or landed aircraft, leaving it where it is.
see_also: [Organic, Teleporter, "Chronoshift.Crushable", "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `Chronoshift.Allow=no`, the [chronosphere](/systems/superweapons/#chronosphere) leaves an object where it is when it stands in the picked area. It is neither moved nor destroyed, including an [`Organic=yes`](/keys/organic/) infantryman that is not [`Teleporter=yes`](/keys/teleporter/), which the chronosphere would otherwise destroy.

The key applies to vehicles, infantry and landed aircraft. On a BuildingType it matters only to a chronosphere that moves structures, [as the superweapons page describes](/systems/superweapons/#chronosphere); the stock chronosphere never moves one.

A skipped object can still be destroyed if another unit lands on it. Set [`Chronoshift.Crushable=no`](/keys/chronoshift.crushable/) to prevent that.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Chronoshift.Allow=no
```
