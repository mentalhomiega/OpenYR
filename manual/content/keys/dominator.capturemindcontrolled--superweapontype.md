---
key: Dominator.CaptureMindControlled
scope: superweapontype
summary: "Chooses whether a psychic dominator takes over units that are mind controlled."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the blast skips any unit that a mind controller holds. With `yes`, the controller lets the unit go first.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.CaptureMindControlled=no
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
