---
key: SW.ForbiddenHouses
scope: superweapontype
summary: "Withholds this superweapon from the listed countries."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "No country"
---

A house whose country is in the list does not hold the weapon, even while it owns a structure that grants it. The list holds country names from `[Countries]`.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.ForbiddenHouses=Russians
```

See [Availability rules](/systems/superweapons/#availability-rules).
