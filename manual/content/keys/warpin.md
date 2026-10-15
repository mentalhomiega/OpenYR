---
key: WarpIn
summary: The animation played where a teleporting object lands.
see_also: [WarpOut, WarpAway, ChronoDelay, "system:tiberium"]
when_omitted:
  kind: value
  value: none
---

This build's teleport locomotor does not play this animation. A unit that teleports, such as a Chrono Legionnaire or a Chrono Miner, plays [WarpOut](/keys/warpout/) at its center where it lands, as gamemd does (0x719788).

```ini title="rulesmd.ini"
[General]
WarpIn=WARPIN ; an AnimType registered in [Animations]
```
