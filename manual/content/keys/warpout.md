---
key: WarpOut
summary: The animation played where a chronosphere picks each unit up and again where it sets it down.
see_also: [ChronoBlast, ChronoInSound, ChronoOutSound, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each unit the [chronosphere](/systems/superweapons/#chronosphere) moves plays this animation at its center when it leaves its cell, 60 frames after the chronosphere fires, and again at its center once it has landed. A unit destroyed before it leaves, such as an infantryman, plays none.

A teleporting unit plays this animation at its center where it stands when it jumps, and again at its center where it lands. [WarpIn](/keys/warpin/) is not played by a teleport in this build.

```ini title="rulesmd.ini"
[General]
WarpOut=MYWARP ; an AnimType registered in [Animations]
```
