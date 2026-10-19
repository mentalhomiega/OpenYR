---
key: Lightning.IgnoreLightningRod
scope: superweapontype
summary: "Stops a lightning storm from favoring structures with LightningRod=yes."
see_also: [LightningRod, "LightningRod.Modifier", LightningRodRange, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the storm's randomly placed clouds form where they were placed, and its bolts do the damage they would to any other structure, whatever [`LightningRod=`](/keys/lightningrod/) and [`LightningRod.Modifier=`](/keys/lightningrod.modifier/) say. The cloud over the target cell is not affected either way.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Type=LightningStorm
Lightning.IgnoreLightningRod=yes
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
