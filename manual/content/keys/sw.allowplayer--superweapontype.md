---
key: SW.AllowPlayer
scope: superweapontype
summary: "Whether a house a person controls may hold this superweapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, a house that a person controls never holds the weapon. A computer house is unaffected; `SW.AllowAI=` decides that case.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AllowPlayer=no
```

See [Availability rules](/systems/superweapons/#availability-rules).
