---
key: Dominator.Ripple
scope: superweapontype
summary: "Chooses whether a psychic dominator sends out its shockwave."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the blast shows no shockwave. Its animations and effects are unchanged.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.Ripple=no
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
