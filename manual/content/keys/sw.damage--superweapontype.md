---
key: SW.Damage
scope: superweapontype
summary: "Sets the damage a generic warhead or missile strike deals."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "-1"
---

A `Type=GenericWarhead` weapon detonates its warhead for this damage; a value of 0 or less does nothing. A `Type=MultiMissile` weapon replaces its payload weapon's damage with it when the value is 0 or more. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Damage=400
```

See [Generic warhead](/systems/superweapons/#generic-warhead).
