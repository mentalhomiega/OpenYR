---
key: MutateExplosionWarhead
summary: "The warhead of the genetic mutator's blast when MutateExplosion=yes."
see_also: [MutateExplosion, CellSpread, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

With [`MutateExplosion=yes`](/keys/mutateexplosion/), the genetic mutator sets off a 10000-damage blast through this warhead, credited to the firing house. Its [`CellSpread`](/keys/cellspread/) sets the area and its `Verses` what it harms. Give it [`InfDeath=9`](/keys/infdeath/) to turn the infantry it kills into brutes.

```ini title="rulesmd.ini"
[SpecialWeapons]
MutateExplosionWarhead=MutateExplosion
```

With no warhead, the blast does nothing.
