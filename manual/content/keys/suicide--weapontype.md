---
key: Suicide
scope: weapontype
label: 'Firing destroys the firer'
see_also: [DeathWeapon, Explodes]
when_omitted:
  kind: value
  value: "no"
---

Firing a weapon with `Suicide=yes` destroys the firer instead of launching a projectile. The firer's [`DeathWeapon`](/keys/deathweapon/#scope-aircrafttype) and explosion then do the damage, so a suicide weapon needs a `DeathWeapon` to hurt anything. The weapon's own `Damage`, `Warhead` and `Projectile` are not used when it fires.

```ini title="rulesmd.ini"
[MyBombTruck] ; example VehicleType
Primary=MyBomb
DeathWeapon=MyBomb
Explodes=yes

[MyBomb] ; example Weapon
Suicide=yes
Range=1
Damage=300
Warhead=MyBombWH
```
