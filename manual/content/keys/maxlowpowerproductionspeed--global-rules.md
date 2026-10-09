---
key: MaxLowPowerProductionSpeed
scope: global-rules
label: Maximum low-power production speed
summary: The highest production speed a house short of power can have.
see_also: ["system:power", MinLowPowerProductionSpeed, LowPowerPenaltyModifier]
when_omitted:
  kind: value
  value: ".9"
---

`MaxLowPowerProductionSpeed` caps a house's production speed while the house is short of power. A house at full power is not capped. The cap applies to any shortfall, however small, so a house that is only slightly short builds at no more than this speed.

The cap is applied after [`MinLowPowerProductionSpeed`](/keys/minlowpowerproductionspeed/). If the minimum is above this value, a house short of power builds at this value.

```ini title="rules.ini"
[General]
MaxLowPowerProductionSpeed=.8  ; no house short of power builds faster than 0.8 speed
```

A value of `1` or more removes the cap, so the shortfall alone sets the speed. Keep this value above 0. At 0, a house short of power has its speed set to 0.01, so its builds take 100 times as long.
