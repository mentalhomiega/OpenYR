---
key: Lightning.ScatterCount
scope: superweapontype
summary: "Sets how many clouds gather around a lightning storm each time."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "1"
---

Each time [`Lightning.ScatterDelay=`](/keys/lightning.scatterdelay/) comes round, the storm tries this many times to place a cloud on a random cell. A try passes over a cell that [`Lightning.Separation=`](/keys/lightning.separation/) rules out, and tries three cells for each cloud before giving it up. A value of 0 or less gathers no clouds.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.ScatterCount=30
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
