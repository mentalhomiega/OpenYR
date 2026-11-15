---
key: Lightning.DebrisMin
scope: superweapontype
summary: "Sets the least debris a lightning bolt throws up."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "2"
---

When a bolt hits bare road, rock, wall or weeds, or changes what stands in its cell, it throws up between this many and [`Lightning.DebrisMax=`](/keys/lightning.debrismax/) animations from [`Lightning.Debris=`](/keys/lightning.debris/). No debris rises where an infantryman stood. If the maximum is not above the minimum, the bolt throws exactly the minimum.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.DebrisMin=0
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
