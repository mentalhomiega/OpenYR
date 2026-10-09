---
key: ChronoRangeMinimum
summary: Distance, in leptons, under which a teleporting object's warp-out wait is only the minimum.
see_also: ["system:tiberium", "Teleporter", "ChronoMinimumDelay", "ChronoTrigger"]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rules.ini"
[General]
ChronoRangeMinimum=0
```

A teleport shorter than this many leptons, 256 to a cell, waits [`ChronoMinimumDelay`](/keys/chronominimumdelay/) frames before it jumps, whatever [`ChronoTrigger`](/keys/chronotrigger/) and [`ChronoDistanceFactor`](/keys/chronodistancefactor/) say. Longer teleports wait as those keys set.
