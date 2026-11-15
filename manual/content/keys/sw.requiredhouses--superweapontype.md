---
key: SW.RequiredHouses
scope: superweapontype
summary: "Limits this superweapon to the listed countries."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "All countries"
---

A house whose country is not in the list does not hold the weapon, even while it owns a structure that grants it. The list holds country names from `[Countries]`. `SW.ForbiddenHouses=` is checked as well.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.RequiredHouses=Americans,Alliance
```

See [Availability rules](/systems/superweapons/#availability-rules).
