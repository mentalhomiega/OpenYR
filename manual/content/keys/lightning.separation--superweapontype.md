---
key: Lightning.Separation
scope: superweapontype
summary: "Sets how far apart the random clouds of a lightning storm must be."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] LightningSeparation"
---

A random cloud is not placed on a cell whose horizontal and vertical distances to an existing cloud add up to less than this many cells. A value of 0 or less turns the rule off. The clouds over the target cell are exempt.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.Separation=2
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
