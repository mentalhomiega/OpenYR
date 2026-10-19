---
key: Mutate.Explosion
scope: superweapontype
summary: "Chooses for one genetic mutator whether it strikes as a blast or mutates the infantry in an area."
see_also: [MutateExplosion, "SW.Damage", "SW.Warhead", "system:superweapons"]
when_omitted:
  kind: inherited
  note: The rules' MutateExplosion.
---

With `yes`, the weapon sets off a blast through its warhead, and the other `Mutate.*` keys and [`SW.Range=`](/keys/sw.range/) have no effect. With `no`, it affects each infantryman in the area. [`SW.Warhead=`](/keys/sw.warhead/) and [`SW.Damage=`](/keys/sw.damage/) replace the rules' warhead and damage in either mode.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Type=GeneticConverter
Mutate.Explosion=no
```

See [Genetic mutator](/systems/superweapons/#genetic-mutator).
