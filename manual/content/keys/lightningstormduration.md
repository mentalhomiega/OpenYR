---
key: LightningStormDuration
summary: "How many frames a lightning storm rages."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "900"
---

A lightning storm rages for this many frames after it breaks, then ends once its last cloud is gone. Enemies of the house that called it lose their radar for as long. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningStormDuration=180
```

At `-1` the storm never ends.
