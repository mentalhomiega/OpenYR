---
key: Chronosphere.AffectsUnwarpable
scope: superweapontype
summary: "Chooses whether a chronoshift moves units that cannot be warped."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the chronosphere leaves a unit whose type sets [`Warpable=no`](/keys/warpable/) where it stands. The key is read from the `Type=ChronoSphere` weapon that picked the units, not from the `Type=ChronoWarp` weapon.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.AffectsUnwarpable=no
```

See [Chronosphere](/systems/superweapons/#chronosphere).
