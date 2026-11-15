---
key: Chronosphere.BlowUnplaceable
scope: superweapontype
summary: "Chooses whether a structure that cannot land at the target is destroyed or stays where it was."
see_also: ["Chronosphere.ReconsiderBuildings", "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

A structure that the chronosphere moves needs its whole foundation on the map and on clear ground at the target. With `yes`, one that does not fit is destroyed where it stood. With `no`, it stays there.

The key is read from the `Type=ChronoSphere` weapon that picked the cells. A structure moved like a vehicle searches for a free foundation near the target first, so it is refused only when none is within six cells.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.BlowUnplaceable=no
```

See [Chronosphere](/systems/superweapons/#chronosphere).
