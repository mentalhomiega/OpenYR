---
key: AICaptureNormal
summary: "The weights for what a computer does with a unit it takes over, when no other condition applies."
see_also: [AICaptureWounded, AICaptureLowPower, AICaptureLowMoney, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

The weights a computer house [rolls against](/systems/mind-control/#what-a-computer-does-with-a-unit) when no other condition applies, to pick what a unit it takes over or gets back does: join the firer's team, go to a grinder, go to a bio reactor, hunt, or do nothing, in that order. Without weights, the unit is left as it is.

```ini title="rulesmd.ini"
[General]
AICaptureNormal=75,5,5,15
```
