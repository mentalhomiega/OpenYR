---
key: LightningCellSpread
summary: "How far around its center a lightning storm scatters its clouds."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "10"
---

A scattered cloud gathers over a cell up to half this many cells from the storm's center along each axis, rounded down. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningCellSpread=10 ; up to 5 cells out
```

At `0` or `1`, scattered clouds gather over the center itself.
