---
key: ChronoInfantryCrush
summary: Sets whether a chronoshifted infantryman destroys a vehicle or aircraft on its landing cell, or dies instead.
see_also: ["Chronoshift.Crushable", "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `ChronoInfantryCrush=no`, an infantryman the [chronosphere](/systems/superweapons/#chronosphere) sets down in a cell with a vehicle or aircraft is destroyed, and the vehicle or aircraft is untouched. By default the infantryman destroys it.

The key does not change what a landing infantryman does to other infantry, or what a landing vehicle does to anything. It applies only when the landing cell has no structure or terrain object. Set [`Chronoshift.Crushable=no`](/keys/chronoshift.crushable/) to protect one type from every kind of arriving unit.

```ini title="rulesmd.ini"
[General]
ChronoInfantryCrush=no
```
