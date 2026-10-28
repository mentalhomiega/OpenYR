---
key: ChronoSparkle1
summary: "The animation played over an object being warped or held at a chronoshift landing, every 24 frames."
see_also: [WarpAway, Temporal, "system:temporal-weapons"]
when_omitted:
  kind: value
  value: none
---

Plays once every 24 game frames over an object while a [temporal](/systems/temporal-weapons/#freezing-the-target) weapon is warping it, and over an object that a chrono teleport holds, from the order to jump until it is released at its landing.

A unit or soldier gets the animation 120 leptons off its position, along both map axes. An aircraft or structure gets it on its position. A structure whose [`MaxNumberOccupants`](/keys/maxnumberoccupants/) is above 0 plays one at the muzzle flash point of each occupant slot instead, one frame apart.

```ini title="rulesmd.ini"
[General]
ChronoSparkle1=MYSPARKLE ; an AnimType registered in [Animations]
```
