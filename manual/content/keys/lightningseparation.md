---
key: LightningSeparation
summary: "How far apart a lightning storm keeps its scattered clouds."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "3"
---

A scattered cloud never gathers over a cell whose distance from an existing cloud, counted as the sum of the two axis distances in cells, is less than this. The storm tries three cells and gives up if all are too close. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningSeparation=3
```

Clouds over the center ignore this.
