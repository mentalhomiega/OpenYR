---
key: KillDriver.KillBelowPercent
summary: "The share of its health a vehicle must be at or below for the warhead to take its driver."
see_also: [KillDriver, ProtectedDriver.MinHealth, "system:warheads"]
when_omitted:
  kind: value
  value: "100%"
---

A vehicle whose strength is above this share of its maximum strength keeps its driver and takes the hit's damage. Write the value as a percentage, such as `34%`, or as a fraction, such as `0.34`. At the default of `100%`, health does not matter.

```ini title="rulesmd.ini"
[MyDriverShotWH] ; example Warhead
KillDriver=yes
KillDriver.KillBelowPercent=34% ; only a vehicle at or below a third of its health
```

The lower of this value and the target's [`ProtectedDriver.MinHealth`](/keys/protecteddriver.minhealth/) applies.
