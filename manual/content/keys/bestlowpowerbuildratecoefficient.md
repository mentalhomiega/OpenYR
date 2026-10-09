---
key: BestLowPowerBuildRateCoefficient
summary: Parsed best-case low-power build coefficient that the engine never uses.
no_effect: true
see_also: ["system:power"]
when_omitted:
  kind: value
  value: ".75"
---

A power shortfall does not read this value. [`MaxLowPowerProductionSpeed`](/keys/maxlowpowerproductionspeed/) caps the speed a shortfall can set, and [the power page](/systems/power/#production) gives the rest of the calculation.
