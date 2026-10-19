---
key: Lightning.Duration
scope: superweapontype
summary: "Sets how many frames a lightning storm rages."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] LightningStormDuration"
---

A `Type=LightningStorm` weapon's storm rages for this many frames once it breaks. `-1` never ends the storm; only a trigger action that ends it does. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.Duration=300
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
