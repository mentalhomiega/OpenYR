---
key: LightningDeferment
summary: "How many frames a lightning storm waits before it breaks."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "250"
---

A lightning storm breaks this many frames after the superweapon is fired. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningDeferment=250
```

At `0` the storm breaks at once.
