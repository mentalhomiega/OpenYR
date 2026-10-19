---
key: SW.Group
scope: superweapontype
summary: "Reads a group number for the weapon; the number has no effect yet."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

The number is stored but nothing uses it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Group=3
```
