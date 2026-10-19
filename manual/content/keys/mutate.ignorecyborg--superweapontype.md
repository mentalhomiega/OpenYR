---
key: Mutate.IgnoreCyborg
scope: superweapontype
summary: "Spares infantry with Cyborg=yes from a genetic mutator that works on an area."
see_also: [Cyborg, "Mutate.IgnoreNotHuman", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an infantryman whose type sets [`Cyborg=yes`](/keys/cyborg/) is left alone by the weapon. The key has no effect when [`Mutate.Explosion=yes`](/keys/mutate.explosion/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Type=GeneticConverter
Mutate.Explosion=no
Mutate.IgnoreCyborg=yes
```

See [Genetic mutator](/systems/superweapons/#genetic-mutator).
