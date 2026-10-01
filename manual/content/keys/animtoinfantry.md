---
key: AnimToInfantry
summary: "The infantry types that MakeInfantry animations turn into."
see_also: [MakeInfantry, InfantryMutate]
when_omitted:
  kind: value
  value: none
---

An animation's [`MakeInfantry`](/keys/makeinfantry/) picks an entry of this list, counting from 0. The genetic mutator's brutes come from it.

```ini title="rulesmd.ini"
[General]
AnimToInfantry=BRUTE
```

An index past the end of the list creates nothing.
