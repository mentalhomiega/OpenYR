---
key: DeathWeapon
scope: aircrafttype
label: Weapon set off on death
see_also: [Explodes, DeathWeaponDamageModifier]
when_omitted:
  kind: value
  value: "none"
---

An [`Explodes=yes`](/keys/explodes/#scope-aircrafttype) object of this type sets off this weapon where it dies, for the weapon's `Damage` times [`DeathWeaponDamageModifier`](/keys/deathweapondamagemodifier/). Without it, the object uses its `Primary` weapon, or the rules' [`DeathWeapon`](/keys/deathweapon/#scope-global-rules) when it has none.

```ini title="rulesmd.ini"
[MYDEMOTRUCK] ; example VehicleType
Explodes=yes
DeathWeapon=MyDemoBomb
```
