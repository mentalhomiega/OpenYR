---
key: AICaptureLowPower
summary: "The weights for what a computer does with a unit it takes over, when the firer's house is short of power."
see_also: [AICaptureNormal, AICaptureWounded, AICaptureLowMoney, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

The weights a computer house [rolls against](/systems/mind-control/#what-a-computer-does-with-a-unit) when the firer's house is short of power, to pick what a unit it takes over or gets back does: join the firer's team, go to a grinder, go to a bio reactor, hunt, or do nothing, in that order. Without weights, the unit is left as it is.

```ini title="rulesmd.ini"
[General]
AICaptureLowPower=15,5,75,5
```
