---
key: Temporal
summary: "Makes a warhead freeze its target and erase it over time instead of damaging it."
see_also: [Warpable, IsRadBeam, "system:temporal-weapons"]
when_omitted:
  kind: value
  value: "no"
---

A weapon with this warhead freezes what it hits and erases it once its warp runs out. It works only as the primary weapon an object has when it is placed on the map. [Temporal weapons](/systems/temporal-weapons/) covers the warp and what makes the firer let go.

```ini title="rulesmd.ini"
[MyChronoBeam] ; example Warhead
Temporal=yes
```
