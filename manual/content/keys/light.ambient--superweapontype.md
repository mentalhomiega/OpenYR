---
key: Light.Ambient
scope: superweapontype
summary: "Reads a lighting setting for the weapon; it has no effect yet."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "-1"
---

The value is stored but nothing uses it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Light.Ambient=100
```
