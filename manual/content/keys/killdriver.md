---
key: KillDriver
summary: "With yes, a hit takes the driver of a vehicle in place of damaging it."
see_also: [KillDriver.Owner, KillDriver.KillBelowPercent, KillDriver.Chance, KillDriver.RemoveVeterancy, ProtectedDriver, "system:warheads"]
when_omitted:
  kind: value
  value: "no"
---

A `KillDriver=yes` warhead takes the driver of a vehicle it hits instead of damaging it. The passengers climb out, and the vehicle goes to the house that [`KillDriver.Owner`](/keys/killdriver.owner/) names. [Taking a vehicle's driver](/systems/warheads/#taking-a-vehicles-driver) lists the conditions and what the hit does to a vehicle that keeps its driver.

```ini title="rulesmd.ini"
[MyDriverShotWH] ; example Warhead
Verses=0%,0%,0%,100%,100%,100%,0%,0%,0%,0%,0%
KillDriver=yes
```

Only a vehicle can lose its driver. The warhead damages infantry, aircraft and structures as it does without the key.
