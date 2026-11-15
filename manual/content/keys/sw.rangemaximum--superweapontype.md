---
key: SW.RangeMaximum
scope: superweapontype
summary: "Sets how far from its launch structure this superweapon may be aimed."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "No limit"
---

The distance in cells from the structure that grants the weapon to the target cell. At least one structure of the house that grants the weapon must be within this distance. A negative value sets no limit. For a `Type=EMPulse` weapon the value also replaces the cannon's weapon range in the targeting cursor. A weapon the house holds without a structure ignores the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.RangeMaximum=20
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
