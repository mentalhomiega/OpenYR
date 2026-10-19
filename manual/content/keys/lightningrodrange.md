---
key: LightningRodRange
summary: "How many cells away a lightning storm cloud is drawn to a structure with LightningRod=yes."
see_also: [LightningRod, "LightningRod.Modifier", "Lightning.IgnoreLightningRod", "system:superweapons"]
when_omitted:
  kind: value
  value: "4"
---

A cloud that a [lightning storm](/systems/superweapons/#lightning-storm) places at random forms over the nearest structure with [`LightningRod=yes`](/keys/lightningrod/) when one is within this many cells of it. `0` turns the pull off. Yuri's Revenge does not read the key.

```ini title="rulesmd.ini"
[General]
LightningRodRange=6
```
