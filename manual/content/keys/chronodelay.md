---
key: ChronoDelay
summary: Frames a teleporting object holds still where it lands before it can act again.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoDistanceFactor", "ChronoMinimumDelay", "ChronoRangeMinimum"]
when_omitted:
  kind: value
  value: "60"
---

```ini title="rules.ini"
[General]
ChronoDelay=60
```

An object that a [`Teleporter`](/keys/teleporter/) moves with the Teleport locomotor holds still for this many game frames after it lands. Its AI waits during the hold, so a Chrono Miner unloads only once the hold has run out. The jump itself is preceded by a wait set by [`ChronoTrigger`](/keys/chronotrigger/), [`ChronoDistanceFactor`](/keys/chronodistancefactor/), [`ChronoMinimumDelay`](/keys/chronominimumdelay/) and [`ChronoRangeMinimum`](/keys/chronorangeminimum/).

This build applies the hold to teleport locomotors only. The chronosphere places its units at once and does not hold them.
