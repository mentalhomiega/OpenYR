---
key: SW.AllowAI
scope: superweapontype
summary: "Whether a house the computer controls may hold this superweapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, a computer house never holds the weapon. A house that a person controls is unaffected; `SW.AllowPlayer=` decides that case.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AllowAI=no
```

See [Availability rules](/systems/superweapons/#availability-rules).
