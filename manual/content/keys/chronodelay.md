---
key: ChronoDelay
summary: Reserved for the chronosphere's post-warp countdown. This build does not read it.
see_also: ["system:tiberium", "Teleporter", "ChronoTrigger", "ChronoDistanceFactor", "ChronoMinimumDelay", "ChronoRangeMinimum"]
when_omitted:
  kind: value
  value: "60"
---

```ini title="rules.ini"
[General]
ChronoDelay=60
```

The hold after a [`Teleporter`](/keys/teleporter/) lands is set by [`ChronoTrigger`](/keys/chronotrigger/), [`ChronoDistanceFactor`](/keys/chronodistancefactor/), [`ChronoMinimumDelay`](/keys/chronominimumdelay/) and [`ChronoRangeMinimum`](/keys/chronorangeminimum/), not by this key. gamemd stores this value in the object's countdown on its chronosphere path (0x719973 to 0x719983), which this build does not model.

This build's teleport locomotor never reads this key, so changing it has no effect here.
