---
key: ProtectedDriver.MinHealth
summary: "The share of its health at or below which a KillDriver warhead can take this vehicle's driver."
see_also: [ProtectedDriver, KillDriver.KillBelowPercent, "system:warheads"]
when_omitted:
  kind: context-dependent
  note: "A type with ProtectedDriver=yes starts at 0, so its driver is never taken. Every other type starts at 1, so health does not matter."
---

A vehicle healthier than this share of its maximum strength keeps its driver. Write the value as a fraction, such as `0.3`, or as a percentage, such as `30%`. The lower of this value and the warhead's [`KillDriver.KillBelowPercent`](/keys/killdriver.killbelowpercent/) applies. A type that sets this key is subject to it whether or not `ProtectedDriver=yes`.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
ProtectedDriver=yes
ProtectedDriver.MinHealth=0.3 ; the driver can be taken once the tank is at 30% health or less
```
