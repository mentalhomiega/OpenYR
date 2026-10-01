---
key: ChronoPlacement
summary: The marker played over the cell a chronosphere's first click picks.
see_also: [ChronoBlast, ChronoBlastDest, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at the center of the picked cell, raised slightly above the ground, or above the bridge when the cell has one. It runs for the loops its animation type sets, and the player sees it only while aiming the [chronosphere](/systems/superweapons/#chronosphere)'s second click. Cancelling that click hides it, and the warp or a new first click lets it finish its current loop and end.

```ini title="rulesmd.ini"
[General]
ChronoPlacement=MYCHRONOMARK ; an AnimType registered in [Animations]
```

A computer house's marker is never shown. With the key unset, the chronosphere still works but marks nothing.
