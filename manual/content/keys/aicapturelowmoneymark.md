---
key: AICaptureLowMoneyMark
summary: "The money below which a computer uses AICaptureLowMoney for a unit it takes over."
see_also: [AICaptureLowMoney, "system:mind-control"]
when_omitted:
  kind: value
  value: "0"
---

A computer house with less money than this [uses `AICaptureLowMoney`](/systems/mind-control/#what-a-computer-does-with-a-unit) to decide what a unit it takes over does.

```ini title="rulesmd.ini"
[General]
AICaptureLowMoneyMark=2000
```
