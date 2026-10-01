---
key: PreDependent
summary: The behavior of the weapon whose first click a PostClick weapon completes.
see_also: [PostClick, PreClick, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Names a superweapon behavior, spelled as a [`Type=`](/keys/type/#scope-superweapontype) value. A [`PostClick=yes`](/keys/postclick/) weapon completes the shot of its house's first weapon in `[SuperWeaponTypes]` order with that `Type=`. A `Type=ChronoSphere` weapon's first click arms targeting for the first `PostClick=yes` weapon whose `PreDependent=` names `ChronoSphere`.

```ini title="rulesmd.ini"
[ChronoWarpSpecial]
Type=ChronoWarp
PostClick=yes
PreDependent=ChronoSphere
```

A value that names no behavior is ignored, and the weapon keeps the value an earlier file set.
