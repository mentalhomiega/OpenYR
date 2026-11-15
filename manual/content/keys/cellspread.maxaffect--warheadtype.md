---
key: CellSpread.MaxAffect
scope: warheadtype
summary: "The most hits one object takes from one blast of this warhead."
see_also: [CellSpread]
when_omitted:
  kind: value
  value: "-1, so an object takes every hit"
---

A blast of a warhead with a `CellSpread` hits an object once for each cell it occupies within the spread, so a large structure takes many hits from one blast. `CellSpread.MaxAffect` keeps only that many hits per object, the ones nearest the blast, and drops the rest. `1` hits each object once. A negative value keeps every hit.

```ini title="rulesmd.ini"
[MyBlastWH] ; example Warhead
CellSpread=3
CellSpread.MaxAffect=1  ; a structure is hit once
```
