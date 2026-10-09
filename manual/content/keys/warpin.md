---
key: WarpIn
summary: The animation played where a teleporting object lands.
see_also: [WarpOut, WarpAway, ChronoDelay, "system:tiberium"]
when_omitted:
  kind: value
  value: none
---

A unit that teleports with a teleport locomotor, such as a Chrono Legionnaire or a Chrono Miner, plays this animation at its center where it lands. A landing moved to a nearby cell plays it on that cell.

```ini title="rulesmd.ini"
[General]
WarpIn=WARPIN ; an AnimType registered in [Animations]
```
