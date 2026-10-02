---
key: Drainable
summary: "Lets a DrainWeapon drain this object."
see_also: [DrainWeapon]
when_omitted:
  kind: value
  value: "no"
---

A [`DrainWeapon`](/keys/drainweapon/) can only drain an object whose type sets `Drainable=yes`. The stock rules set it on power plants, refineries and some base defenses.

```ini title="rulesmd.ini"
[MYPOWERPLANT] ; example BuildingType
Drainable=yes
```
