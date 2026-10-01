---
key: PercentAtMax
summary: "The share of its damage a blast deals at the edge of its reach."
see_also: [CellSpread, Verses, "system:warheads"]
when_omitted:
  kind: value
  value: "1"
---

A blast deals its full damage at the point of impact and this share of it at the edge of the reach that [`CellSpread`](/keys/cellspread/) sets. Damage falls in a straight line between the two. [How distance thins the damage](/systems/warheads/#how-distance-thins-the-damage) gives the formula and an example.

```ini title="rulesmd.ini"
[MyShellWH] ; example WarheadType
CellSpread=2
PercentAtMax=.25 ; a quarter of the damage two cells out
```

At `1`, the default, damage does not thin. A value above `1` makes damage grow toward the edge. Damage never falls below zero.
