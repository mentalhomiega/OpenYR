---
key: DeathWeaponDamageModifier
summary: "Multiplies the damage of the weapon an exploding object sets off as it dies."
see_also: [DeathWeapon, Explodes]
when_omitted:
  kind: value
  value: "1.0"
---

An [`Explodes=yes`](/keys/explodes/#scope-aircrafttype) object of this type sets off its [death weapon](/keys/deathweapon/#scope-aircrafttype), or its `Primary` weapon, for that weapon's `Damage` times this value, rounded down.

```ini title="rulesmd.ini"
[MYDEMOTRUCK] ; example VehicleType
DeathWeaponDamageModifier=1.5
```
