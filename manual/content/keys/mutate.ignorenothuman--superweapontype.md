---
key: Mutate.IgnoreNotHuman
scope: superweapontype
summary: "Spares infantry with NotHuman=yes from a genetic mutator that works on an area."
see_also: [NotHuman, "Mutate.IgnoreCyborg", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an infantryman whose type sets [`NotHuman=yes`](/keys/nothuman/) is left alone by the weapon. The key has no effect when [`Mutate.Explosion=yes`](/keys/mutate.explosion/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Type=GeneticConverter
Mutate.Explosion=no
Mutate.IgnoreNotHuman=yes
```

See [Genetic mutator](/systems/superweapons/#genetic-mutator).
