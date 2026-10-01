---
key: MutateWarhead
summary: "The warhead the genetic mutator kills infantry with when MutateExplosion=no."
see_also: [MutateExplosion, InfDeath, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

With [`MutateExplosion=no`](/keys/mutateexplosion/), every infantryman on the genetic mutator's target cell and the eight cells around it takes damage equal to his full strength through this warhead, ignoring armor. Give it [`InfDeath=9`](/keys/infdeath/) to turn them into brutes.

```ini title="rulesmd.ini"
[SpecialWeapons]
MutateWarhead=Mutate
```

Vehicles and structures are not touched.
