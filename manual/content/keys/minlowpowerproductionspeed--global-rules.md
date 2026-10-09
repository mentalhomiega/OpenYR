---
key: MinLowPowerProductionSpeed
scope: global-rules
label: Minimum low-power production speed
summary: The floor on a house's production speed, which divides its build times.
see_also: ["system:power", MaxLowPowerProductionSpeed, LowPowerPenaltyModifier]
when_omitted:
  kind: value
  value: ".5"
---

`MinLowPowerProductionSpeed` is the lowest production speed a house can have. The engine raises a house's speed to this value when the power calculation leaves it lower, and each build time is divided by that speed. With `.5`, the slowest build takes twice its normal time, unless the maximum is lower.

With the default [`LowPowerPenaltyModifier`](/keys/lowpowerpenaltymodifier/), a house with no output builds at exactly this speed, unless [`MaxLowPowerProductionSpeed`](/keys/maxlowpowerproductionspeed/) is lower. [The power page](/systems/power/#production) gives the full calculation.

```ini title="rules.ini"
[General]
MinLowPowerProductionSpeed=.6  ; with the default penalty, a house with no output builds at 0.6 speed
```

:::caution[Values above 1 speed up houses at full power]
The floor also applies at full power. With `MinLowPowerProductionSpeed=2`, a house at full power builds in half the time. A house short of power is still held to [`MaxLowPowerProductionSpeed`](/keys/maxlowpowerproductionspeed/).
:::

Keep this value above 0. At 0, a house whose speed works out to exactly 0 is given 0.01 instead, so its builds take 100 times as long.
