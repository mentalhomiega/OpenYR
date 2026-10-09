---
key: ChronoTrigger
summary: Whether a teleporting object's warp-out wait grows with the distance it teleports.
see_also: ["system:tiberium", "Teleporter", "ChronoDistanceFactor", "ChronoMinimumDelay", "ChronoDelay"]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="rules.ini"
[General]
ChronoTrigger=yes
```

With `yes`, a teleporting object waits its distance divided by [`ChronoDistanceFactor`](/keys/chronodistancefactor/) before it jumps, and never less than [`ChronoMinimumDelay`](/keys/chronominimumdelay/). With `no`, every jump waits only `ChronoMinimumDelay`. Under [`ChronoRangeMinimum`](/keys/chronorangeminimum/) the wait is always `ChronoMinimumDelay`.
