---
key: LightningScatterDelay
summary: "How often a cloud gathers somewhere around a lightning storm."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "10"
---

While a lightning storm rages, a cloud gathers over a random cell near its center on every frame number divisible by this value. [`LightningCellSpread`](/keys/lightningcellspread/) and [`LightningSeparation`](/keys/lightningseparation/) decide where. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningScatterDelay=5
```

At `0` or below, no clouds gather around the center.
