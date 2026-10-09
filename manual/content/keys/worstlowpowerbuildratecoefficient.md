---
key: WorstLowPowerBuildRateCoefficient
summary: Parsed worst-case low-power build coefficient that the engine never uses.
no_effect: true
see_also: ["system:power"]
when_omitted:
  kind: value
  value: ".3"
---

A power shortfall does not read this value. [`MinLowPowerProductionSpeed`](/keys/minlowpowerproductionspeed/) sets the floor on a house's speed, and [the power page](/systems/power/#production) gives the rest of the calculation.
