---
key: CellRangefinding
summary: "Measures the weapon's range from the center of the firer's cell rather than from the firer."
see_also: [Range]
when_omitted:
  kind: value
  value: "no"
---

With `CellRangefinding=yes`, the weapon's [`Range`](/keys/range/#scope-weapontype) is measured from the center of the cell the firer stands in, at ground level or on the bridge deck there, rather than from the firer's own position. A soldier standing in a corner of its cell therefore reaches the same targets as one in the middle of it.

```ini title="rulesmd.ini"
[MyDefuseKit] ; example Weapon
Range=1.5
CellRangefinding=yes
```
