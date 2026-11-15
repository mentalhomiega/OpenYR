---
key: Chronosphere.KillOrganic
scope: superweapontype
summary: "Chooses whether a chronoshift destroys organic units."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, an [`Organic=yes`](/keys/organic/) unit that is not [`Teleporter=yes`](/keys/teleporter/) is destroyed instead of moved. With `no`, it moves like any other unit. The key is read from the `Type=ChronoSphere` weapon that picked the units, not from the `Type=ChronoWarp` weapon.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.KillOrganic=no
```

See [Chronosphere](/systems/superweapons/#chronosphere).
