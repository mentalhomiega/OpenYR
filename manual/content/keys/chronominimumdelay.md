---
key: ChronoMinimumDelay
summary: Shortest hold, in frames, that a teleporting object keeps after it lands.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoRangeMinimum", "ChronoDelay"]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rules.ini"
[General]
ChronoMinimumDelay=16
```

A teleporting object holds at least this many frames after it lands, however short the distance. The hold is also this value whenever [`ChronoTrigger`](/keys/chronotrigger/) is `no`, and whenever the distance is under [`ChronoRangeMinimum`](/keys/chronorangeminimum/).
