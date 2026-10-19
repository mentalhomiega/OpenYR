---
key: Lightning.DebrisMax
scope: superweapontype
summary: "Sets the most debris a lightning bolt throws up."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "4"
---

See [`Lightning.DebrisMin=`](/keys/lightning.debrismin/). A bolt throws a random whole number of animations from the minimum to this value.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.DebrisMax=0
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
