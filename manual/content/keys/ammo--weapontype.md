---
key: Ammo
scope: weapontype
label: Rounds one shot uses
see_also: [NoAmmoWeapon, Reload]
when_omitted:
  kind: value
  value: "1"
---

`Ammo` on a weapon is an Ares key: the number of rounds of its firer's [`Ammo`](#scope-aircrafttype) one shot of this weapon uses. Each shot of a [`Burst`](/keys/burst/) uses them again. An object holding fewer rounds than the weapon needs does not fire it and waits for its rounds to reload. An object with unlimited ammunition, `Ammo=-1` on its type, fires whatever the weapon's value.

`Ammo=0` uses no rounds, so the weapon fires even from an empty object, and its shots do not restart the object's [`Reload`](/keys/reload/) countdown. A negative value counts as `0`.

Aircraft spend one round a shot whatever this key says, as Ares does not support it on aircraft. A [`NoAmmoWeapon`](/keys/noammoweapon/) still fires once ammunition is low whatever its own value.

```ini title="rulesmd.ini"
[105mm]
Ammo=2   ; each shell uses two of the tank's rounds

[TankMG]
Ammo=0   ; the machine gun never runs dry
```
