---
key: ChronoBlastDest
summary: The animation played over the target of a chronosphere warp.
see_also: [ChronoBlast, WarpOut, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at the center of the warp's target cell, raised slightly above the ground, or above the bridge when the cell has one, when the [chronosphere](/systems/superweapons/#chronosphere) warp fires. [`ChronoBlast`](/keys/chronoblast/) plays over the picked cell at the same moment.

```ini title="rulesmd.ini"
[General]
ChronoBlastDest=MYCHRONOBLAST2 ; an AnimType registered in [Animations]
```

With the key unset, the warp still moves the units but shows nothing at the target.
