---
key: ChronoBlast
summary: The animation played over the cell a chronosphere picked, as the warp fires.
see_also: [ChronoBlastDest, ChronoPlacement, WarpOut, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at the center of the picked cell, raised slightly above the ground, or above the bridge when the cell has one, when the [chronosphere](/systems/superweapons/#chronosphere) warp fires. [`ChronoBlastDest`](/keys/chronoblastdest/) plays at the target at the same moment.

```ini title="rulesmd.ini"
[General]
ChronoBlast=MYCHRONOBLAST ; an AnimType registered in [Animations]
```

With the key unset, the warp still moves the units but shows nothing where they leave.
