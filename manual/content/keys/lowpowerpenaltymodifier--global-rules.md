---
key: LowPowerPenaltyModifier
scope: global-rules
label: Low-power penalty modifier
summary: The multiplier on a house's power shortfall, which sets how far its production speed falls.
see_also: ["system:power", MinLowPowerProductionSpeed, MaxLowPowerProductionSpeed]
when_omitted:
  kind: value
  value: "1"
---

`LowPowerPenaltyModifier` multiplies a house's power shortfall before the shortfall sets its production speed. The shortfall is 1 minus the house's [power fraction](/systems/power/#the-power-fraction), and the speed is 1 minus the shortfall times this value. A house at full power has no shortfall, so this value does not affect it.

With `1`, a house at 80 percent power has a shortfall of 0.2 and a speed of 0.8, so its builds take a quarter longer. With `2`, the same house has a speed of 0.6, and its builds take two-thirds longer. At `0`, the shortfall has no effect, so a house short of power has a speed of 1 before the [minimum and maximum](/systems/power/#production) apply.

```ini title="rules.ini"
[General]
LowPowerPenaltyModifier=2  ; a 20 percent shortfall gives 0.6 speed
```

Keep this value at 0 or above.
