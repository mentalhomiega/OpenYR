---
key: ChronoDistanceFactor
summary: Leptons a teleporting object covers for each frame of its warp-out wait.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoMinimumDelay", "ChronoDelay"]
when_omitted:
  kind: value
  value: "32"
---

```ini title="rules.ini"
[General]
ChronoDistanceFactor=48
```

With [`ChronoTrigger`](/keys/chronotrigger/) set to `yes`, the warp-out wait of a teleporting object is its distance in leptons, 256 to a cell, divided by this value. A value of `0` or less turns the distance wait off, so the wait is `ChronoMinimumDelay`. Each frame of wait covers this many leptons: 32 gives eight frames for every cell travelled.
