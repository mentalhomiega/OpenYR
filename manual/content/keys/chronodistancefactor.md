---
key: ChronoDistanceFactor
summary: Leptons a teleporting object covers for each frame of its hold after landing.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoMinimumDelay", "ChronoDelay"]
when_omitted:
  kind: value
  value: "32"
---

```ini title="rules.ini"
[General]
ChronoDistanceFactor=48
```

With [`ChronoTrigger`](/keys/chronotrigger/) set to `yes`, the hold of a teleporting object after it lands is its distance in leptons, 256 to a cell, divided by this value. A value of `0` or less turns the distance hold off, so the hold is `ChronoMinimumDelay`. Each frame of hold covers this many leptons: 32 gives eight frames for every cell travelled.
