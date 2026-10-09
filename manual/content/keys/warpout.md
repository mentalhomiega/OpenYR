---
key: WarpOut
summary: The animation played where a chronosphere picks each unit up and again where it sets it down.
see_also: [ChronoBlast, ChronoInSound, ChronoOutSound, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each unit the [chronosphere](/systems/superweapons/#chronosphere) moves plays this animation at its center just before it leaves, and again at its center once it has landed. A unit destroyed before it leaves, such as an infantryman, plays none.

A teleporting unit also plays this animation at its center when it is ordered to teleport, where it stands at that moment. The animation that plays where it lands is [WarpIn](/keys/warpin/).

```ini title="rulesmd.ini"
[General]
WarpOut=MYWARP ; an AnimType registered in [Animations]
```
