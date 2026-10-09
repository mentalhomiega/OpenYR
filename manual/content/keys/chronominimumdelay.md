---
key: ChronoMinimumDelay
summary: Shortest warp-out wait, in frames, before a teleporting object jumps.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoRangeMinimum", "ChronoDelay"]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rules.ini"
[General]
ChronoMinimumDelay=16
```

A teleporting object waits at least this many frames before it jumps, however short the distance. The wait is also this value whenever [`ChronoTrigger`](/keys/chronotrigger/) is `no`, and whenever the distance is under [`ChronoRangeMinimum`](/keys/chronorangeminimum/).
