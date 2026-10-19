---
key: Light.Enabled
scope: superweapontype
summary: "Reads a lighting setting for the weapon; it has no effect yet."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

The value is stored but nothing uses it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Light.Enabled=yes
```
