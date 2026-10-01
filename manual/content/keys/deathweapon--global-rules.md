---
key: DeathWeapon
scope: global-rules
label: Default weapon set off on death
see_also: [Explodes]
when_omitted:
  kind: value
  value: "none"
---

An [`Explodes=yes`](/keys/explodes/#scope-aircrafttype) object whose type has neither its own [`DeathWeapon`](/keys/deathweapon/#scope-aircrafttype) nor a `Primary` weapon sets off this weapon where it dies, for half its type's `Strength`.

```ini title="rulesmd.ini"
[CombatDamage]
DeathWeapon=DefaultDeathWeapon
```
