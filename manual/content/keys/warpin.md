---
key: WarpIn
summary: The animation played where a teleporting object lands.
see_also: [WarpOut, WarpAway, ChronoDelay, "system:tiberium"]
when_omitted:
  kind: value
  value: none
---

A unit that teleports with a teleport locomotor, such as a Chrono Legionnaire or a Chrono Miner, plays this animation at its center on the cell where it lands. It plays nothing when the landing cell is the one it was ordered to, and nothing when the key is omitted.

```ini title="rulesmd.ini"
[General]
WarpIn=WARPIN ; an AnimType registered in [Animations]
```
