---
key: AICaptureWoundedMark
summary: "The health share below which a computer uses AICaptureWounded for a unit it takes over."
see_also: [AICaptureWounded, "system:mind-control"]
when_omitted:
  kind: value
  value: "0"
---

A unit whose health is below this share of its maximum [is decided with `AICaptureWounded`](/systems/mind-control/#what-a-computer-does-with-a-unit), unless its new owner is short of money or power.

```ini title="rulesmd.ini"
[General]
AICaptureWoundedMark=.25
```
