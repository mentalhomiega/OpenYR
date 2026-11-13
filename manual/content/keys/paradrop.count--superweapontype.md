---
key: ParaDrop.Count
scope: superweapontype
summary: "Sets how many planes a paradrop weapon sends."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "1"
---

Each plane carries the whole [`ParaDrop.Types=`](/keys/paradrop.types/) load, unless the weapon gives a plane after the first its own with the `ParaDrop.Plane2.Types=` form described there. The key has no effect on a weapon that uses the side's lists.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
ParaDrop.Count=2
```

See [Paradrops](/systems/superweapons/#paradrops).
