---
key: MutateExplosion
summary: "Whether the genetic mutator strikes as a blast or mutates the infantry on nine cells."
see_also: [MutateExplosionWarhead, MutateWarhead, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `MutateExplosion=yes`, the genetic mutator sets off a 10000-damage blast through [`MutateExplosionWarhead`](/keys/mutateexplosionwarhead/). Otherwise it deals each infantryman on and around the target cell its full strength through [`MutateWarhead`](/keys/mutatewarhead/). [Genetic mutator](/systems/superweapons/#genetic-mutator) covers both.

```ini title="rulesmd.ini"
[General]
MutateExplosion=yes
```

Either way, a warhead with `InfDeath=9` is what turns the dead into brutes.
