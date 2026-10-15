---
key: ChronoDelay
summary: Frames a unit moved by a chronosphere waits on its cell before it lands.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoDistanceFactor", "ChronoMinimumDelay", "ChronoRangeMinimum"]
when_omitted:
  kind: value
  value: "60"
---

```ini title="rules.ini"
[General]
ChronoDelay=60
```

A unit that a chronosphere moves stays on its cell under [`WarpOut`](/keys/warpout/) for this many frames, then lands with [`WarpIn`](/keys/warpin/). gamemd stores this value in the unit's countdown on the chronosphere path (0x719973 to 0x719983), and the warp-out timer counts it down (0x719B23).

The hold after a [`Teleporter`](/keys/teleporter/) lands is set by [`ChronoTrigger`](/keys/chronotrigger/), [`ChronoDistanceFactor`](/keys/chronodistancefactor/), [`ChronoMinimumDelay`](/keys/chronominimumdelay/) and [`ChronoRangeMinimum`](/keys/chronorangeminimum/), not by this key. A teleporter's own warp does not wait for this key.
