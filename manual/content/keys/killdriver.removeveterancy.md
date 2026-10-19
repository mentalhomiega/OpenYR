---
key: KillDriver.RemoveVeterancy
summary: "With yes, a vehicle whose driver was killed goes back to the rookie rank."
see_also: [KillDriver, "system:veterancy"]
when_omitted:
  kind: value
  value: "no"
---

The vehicle's rank is reset when its driver is taken. A vehicle whose driver is not taken keeps its rank.

```ini title="rulesmd.ini"
[MyDriverShotWH] ; example Warhead
KillDriver=yes
KillDriver.RemoveVeterancy=yes
```
