---
key: SW.Warhead
scope: superweapontype
summary: "Names the warhead a generic warhead or missile strike detonates."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A `Type=GenericWarhead` weapon detonates this warhead on the target cell and does nothing without it. A `Type=MultiMissile` weapon replaces its payload weapon's warhead with it. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Warhead=HE
```

See [Generic warhead](/systems/superweapons/#generic-warhead).
