---
key: LightningRod.Modifier
scope: buildingtype
summary: "Scales the damage a lightning storm bolt does to a structure with LightningRod=yes."
see_also: [LightningRod, LightningRodRange, "Lightning.IgnoreLightningRod", "system:superweapons"]
when_omitted:
  kind: value
  value: "1.0"
---

While [`LightningRod=yes`](/keys/lightningrod/) is set, a bolt of a lightning storm does this share of its damage to the structure, rounded down. `0.5` halves it and `0` makes the structure immune. A weapon with [`Lightning.IgnoreLightningRod=yes`](/keys/lightning.ignorelightningrod/) does not use the key. Ion storm bolts do not use it either.

```ini title="rulesmd.ini"
[GAPOWR] ; example BuildingType
LightningRod=yes
LightningRod.Modifier=0.5
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
