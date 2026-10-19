---
key: SW.RangeMinimum
scope: superweapontype
summary: "Sets how close to its launch structure this superweapon may be aimed."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "No limit"
---

The distance in cells from the structure that grants the weapon to the target cell. At least one structure of the house that grants the weapon must be at least this far away. A negative value sets no limit. A weapon the house holds without a structure ignores the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.RangeMinimum=4
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
