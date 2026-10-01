---
key: AICaptureLowMoney
summary: "The weights for what a computer does with a unit it takes over, when the firer's house has less money than AICaptureLowMoneyMark."
see_also: [AICaptureNormal, AICaptureWounded, AICaptureLowPower, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

The weights a computer house [rolls against](/systems/mind-control/#what-a-computer-does-with-a-unit) when the firer's house has less money than AICaptureLowMoneyMark, to pick what a unit it takes over or gets back does: join the firer's team, go to a grinder, go to a bio reactor, hunt, or do nothing, in that order. Without weights, the unit is left as it is.

```ini title="rulesmd.ini"
[General]
AICaptureLowMoney=15,75,5,5
```
