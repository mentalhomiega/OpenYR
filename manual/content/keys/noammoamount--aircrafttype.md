---
key: NoAmmoAmount
scope: aircrafttype
summary: "The ammunition at or below which NoAmmoWeapon takes over."
see_also: [NoAmmoWeapon, Ammo]
when_omitted:
  kind: value
  value: "0, so NoAmmoWeapon takes over when the ammunition is gone"
---

An object whose type sets [`NoAmmoWeapon`](/keys/noammoweapon/) fires only that weapon while its ammunition is at or below this number. With `1`, the last shell is kept back and the object fires the other weapon from its last shell on. The key does nothing without `NoAmmoWeapon`.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
NoAmmoWeapon=1
NoAmmoAmount=1
```
