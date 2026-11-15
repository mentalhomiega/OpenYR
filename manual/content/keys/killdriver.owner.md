---
key: KillDriver.Owner
summary: "The house that takes a vehicle whose driver a KillDriver warhead killed."
see_also: [KillDriver, "system:warheads"]
when_omitted:
  kind: value
  value: "special"
---

`special` gives the vehicle to the house of the `Special` country, `neutral` to the house of the `Neutral` country, and `civilian` to the first house of the Civilian side. The words are not case-sensitive. Any other word leaves the default, `special`.

```ini title="rulesmd.ini"
[MyDriverShotWH] ; example Warhead
KillDriver=yes
KillDriver.Owner=civilian
```

When the game has no house for the value, or the vehicle already belongs to that house, the driver is not taken and the vehicle takes the hit's damage.
