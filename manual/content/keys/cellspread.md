---
key: CellSpread
summary: "How many cells a blast from this warhead reaches."
see_also: [PercentAtMax, Verses, "system:warheads"]
when_omitted:
  kind: value
  value: "0"
---

A blast reaches `CellSpread` cells from the point of impact, and only objects that close can be damaged. Damage thins with distance out to that reach, as [`PercentAtMax`](/keys/percentatmax/) sets. [What a blast reaches](/systems/warheads/#what-a-blast-reaches) covers how objects are found and measured.

```ini title="rulesmd.ini"
[MyShellWH] ; example WarheadType
CellSpread=1.5 ; reaches a cell and a half
```

Fractions count. At `0`, the default, a blast damages only what it hits exactly, such as the target of a direct shot. A wide blast hits a large structure once for each of its cells in reach, so it loses more strength than a small one.
