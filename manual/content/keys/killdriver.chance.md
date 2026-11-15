---
key: KillDriver.Chance
summary: "The chance that a hit takes the driver of a vehicle that has met every other condition."
see_also: [KillDriver, "system:warheads"]
when_omitted:
  kind: value
  value: "100%"
---

The chance is rolled for each vehicle each hit, after the other conditions are met, and is rounded to a whole percent. A vehicle that wins the roll loses its driver; one that loses it takes the hit's damage. At `100%` or more, no roll is made.

```ini title="rulesmd.ini"
[MyDriverShotWH] ; example Warhead
KillDriver=yes
KillDriver.Chance=25%
```
