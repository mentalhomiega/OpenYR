---
key: Dominator.CapturePermaMindControlled
scope: superweapontype
summary: "Chooses whether a psychic dominator takes over units that are permanently mind controlled."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the blast skips any unit that an earlier dominator blast took over for good.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.CapturePermaMindControlled=no
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
