---
key: DeathWeapon.SuppressVehicles
scope: warheadtype
summary: "With yes, vehicles this warhead kills set off no death weapon."
see_also: [DeathWeapon.Suppress, DeathWeapon.SuppressInfantry]
when_omitted:
  kind: value
  value: "no"
---

`DeathWeapon.SuppressVehicles` is an Ares key. With `yes`, every vehicle this warhead kills dies without setting off its [death weapon](/keys/deathweapon/#scope-aircrafttype); aircraft and buildings are not vehicles here. With `no`, the warhead still spares the vehicle types its [`DeathWeapon.Suppress`](/keys/deathweapon.suppress/) list names. A balloon vehicle this warhead shoots down sets off no death weapon when it lands.

```ini title="rulesmd.ini"
[MyDefuseWH] ; example Warhead
DeathWeapon.SuppressVehicles=yes
```
