---
key: Chronosphere.KillCargo
scope: superweapontype
summary: "Chooses whether a chronoshift destroys the passengers of the units it moves."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, every passenger of a unit that the chronosphere moves is destroyed, and the unit arrives empty. The key is read from the `Type=ChronoSphere` weapon that picked the units, not from the `Type=ChronoWarp` weapon.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.KillCargo=yes
```

See [Chronosphere](/systems/superweapons/#chronosphere).
