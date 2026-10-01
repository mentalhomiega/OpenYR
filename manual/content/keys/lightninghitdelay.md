---
key: LightningHitDelay
summary: "How often a cloud gathers over the center of a lightning storm."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "90"
---

While a lightning storm rages, a cloud gathers over its center on every frame number divisible by this value, and a bolt follows. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningHitDelay=10
```

At `0` or below, no clouds gather over the center.
