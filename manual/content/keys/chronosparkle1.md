---
key: ChronoSparkle1
summary: "The animation played over an object being warped, every 24 frames."
see_also: [WarpAway, Temporal, "system:temporal-weapons"]
when_omitted:
  kind: value
  value: none
---

Plays 120 leptons off the position of an object a [temporal](/systems/temporal-weapons/#freezing-the-target) weapon is warping, along both map axes, once every 24 game frames while the warp lasts.

```ini title="rulesmd.ini"
[General]
ChronoSparkle1=MYSPARKLE ; an AnimType registered in [Animations]
```
