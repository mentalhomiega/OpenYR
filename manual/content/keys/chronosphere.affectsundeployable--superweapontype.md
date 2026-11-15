---
key: Chronosphere.AffectsUndeployable
scope: superweapontype
summary: "Makes the chronosphere move every structure that can undeploy, as if it were a vehicle."
see_also: ["Chronosphere.ReconsiderBuildings", "Chronoshift.IsVehicle", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a structure whose type sets [`UndeploysInto=`](/keys/undeploysinto/) is moved like a vehicle, whatever [`SW.AffectsTarget=`](/keys/sw.affectstarget/) and [`Chronoshift.IsVehicle=`](/keys/chronoshift.isvehicle/) say. [`SW.AffectsHouse=`](/keys/sw.affectshouse/), [`Chronoshift.Allow=no`](/keys/chronoshift.allow/), the Iron Curtain and [`Chronosphere.AffectsUnwarpable=no`](/keys/chronosphere.affectsunwarpable/) still exclude it.

The key is read from the `Type=ChronoSphere` weapon that picked the cells.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.AffectsUndeployable=yes
```

See [Chronosphere](/systems/superweapons/#chronosphere).
