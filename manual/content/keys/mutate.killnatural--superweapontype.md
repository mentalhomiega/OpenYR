---
key: Mutate.KillNatural
scope: superweapontype
summary: "Kills infantry with Natural=yes outright when a genetic mutator deals damage through its warhead."
see_also: [Natural, "SW.Damage", "SW.Warhead", "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

A genetic mutator that sets [`SW.Damage=`](/keys/sw.damage/) or [`SW.Warhead=`](/keys/sw.warhead/) damages infantry through the warhead, so the warhead's `Verses` apply. With `yes`, an infantryman whose type sets [`Natural=yes`](/keys/natural/) takes its full strength instead and dies. With `no`, it takes the weapon's damage like the others. A mutator that sets neither key already kills every infantryman it reaches, and the key has no effect when [`Mutate.Explosion=yes`](/keys/mutate.explosion/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Type=GeneticConverter
Mutate.Explosion=no
SW.Damage=100
Mutate.KillNatural=no
```

See [Genetic mutator](/systems/superweapons/#genetic-mutator).
