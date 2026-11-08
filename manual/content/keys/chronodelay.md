---
key: ChronoDelay
summary: Frames a unit holds after a chronosphere set it down on a cell other than the one it was sent to.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoDistanceFactor", "ChronoMinimumDelay", "ChronoRangeMinimum"]
when_omitted:
  kind: value
  value: "60"
---

```ini title="rulesmd.ini"
[General]
ChronoDelay=60
```

A unit that a chronosphere moves waits 60 frames on its cell, then leaves it, and is set down at the destination. This key does not change that wait. Whatever stands on the destination is destroyed when the unit leaves, not when the chronosphere fires.

When a structure or tree stands on the destination, or the destination is the far edge of a bridge's under-strip, the unit lands on a nearby cell instead and holds there for this many frames, unable to act or to be picked by another chronosphere. The unit keeps that hold for every later chronosphere warp, landed where it was sent or not, and a unit that has never had its landing moved lands with no hold.

The hold after a [`Teleporter`](/keys/teleporter/) lands is set by [`ChronoTrigger`](/keys/chronotrigger/), [`ChronoDistanceFactor`](/keys/chronodistancefactor/), [`ChronoMinimumDelay`](/keys/chronominimumdelay/) and [`ChronoRangeMinimum`](/keys/chronorangeminimum/), not by this key.
