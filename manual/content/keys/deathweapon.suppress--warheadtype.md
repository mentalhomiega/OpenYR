---
key: DeathWeapon.Suppress
scope: warheadtype
summary: "Object types whose death weapon does not go off when this warhead kills them."
see_also: [DeathWeapon.SuppressInfantry, DeathWeapon.SuppressVehicles, DeathWeapon, Explodes]
when_omitted:
  kind: value
  value: "none"
---

`DeathWeapon.Suppress` is an Ares key. It takes a list of infantry, vehicle, aircraft or building types. An object of a listed type that this warhead kills sets off no [death weapon](/keys/deathweapon/#scope-aircrafttype), whatever its `Explodes` setting or current weapon. A listed object that this warhead only damages, and another warhead later kills, still sets off its death weapon. An aircraft or a crashing balloon that this warhead shot down sets off none when it reaches the ground either, unless a saved game was loaded while it fell.

The list adds to [`DeathWeapon.SuppressInfantry`](/keys/deathweapon.suppressinfantry/) and [`DeathWeapon.SuppressVehicles`](/keys/deathweapon.suppressvehicles/): a type is spared its blast when it is listed or when its kind's switch is on.

```ini title="rulesmd.ini"
[MyCleanKillWH] ; example Warhead
DeathWeapon.Suppress=DEMOTRUCK,TERROR
```
