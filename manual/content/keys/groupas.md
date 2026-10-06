---
key: GroupAs
summary: Names the group the Select Same Type command puts this type in.
when_omitted:
  kind: value
  value: "none; the type is grouped under its own ID"
---

The [Select Same Type](/commands/selecttype/) command selects every object in the same group as an object in the current selection. A type's group is its `GroupAs` text, or its own ObjectType ID when `GroupAs` is not set. Two types share a group when their group text matches without regard to letter case, so a vehicle and an infantryman can share one.

```ini title="rulesmd.ini"
[APOC]  ; example VehicleType
GroupAs=HEAVYTANK
[RHINO] ; example VehicleType
GroupAs=HEAVYTANK
```

With this example, selecting either tank and pressing the command selects both kinds. A type with no `GroupAs` is grouped under its ID, so it also joins any type whose `GroupAs` equals that ID.

The Ares `TypeSelectUseDeploy` key is not read.
