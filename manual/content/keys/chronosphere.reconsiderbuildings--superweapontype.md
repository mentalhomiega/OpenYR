---
key: Chronosphere.ReconsiderBuildings
scope: superweapontype
summary: "Chooses whether a structure flagged as a deployed vehicle counts as a vehicle for the chronosphere."
see_also: ["Chronoshift.IsVehicle", "Chronosphere.AffectsUndeployable", "SW.AffectsTarget", "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes`, a structure whose type sets [`Chronoshift.IsVehicle=yes`](/keys/chronoshift.isvehicle/) is treated as a vehicle: [`SW.AffectsTarget=`](/keys/sw.affectstarget/) must allow units for the chronosphere to move it, and it lands on the nearest free foundation. With `no`, every structure counts as a structure, and `SW.AffectsTarget=` must list `buildings`.

The key is read from the `Type=ChronoSphere` weapon that picked the cells. A structure that [`Chronoshift.Allow=no`](/keys/chronoshift.allow/) forbids is never moved.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.ReconsiderBuildings=no
```

See [Chronosphere](/systems/superweapons/#chronosphere).
