---
key: Dominator.Capture
scope: superweapontype
summary: "Chooses whether a psychic dominator takes units over."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the blast deals its damage and plays its animations but takes no unit over.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.Capture=no
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
