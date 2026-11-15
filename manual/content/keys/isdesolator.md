---
key: IsDesolator
summary: "Stops the soldier's AreaFire deploy weapon from aiming at the soldier's own cell."
see_also: [DeployFireWeapon, AreaFire, IsCow]
when_omitted:
  kind: value
  value: "yes for the type named DESO, otherwise no"
---

`IsDesolator` is an Ares key. When a soldier deploys and its [`DeployFireWeapon`](/keys/deployfireweapon/) has `AreaFire=yes`, it normally aims the weapon at its own cell. With `IsDesolator=yes` it does not: it deploys and takes no target of its own from the weapon, as the Desolator does.

The default is `yes` for a type whose name is `DESO` and `no` for every other, so stock rules behave as before. Nothing else about the type changes. Radiation or other effects of the weapon are set by the weapon and its warhead.

```ini title="rulesmd.ini"
[DESO]
IsDesolator=yes
```
