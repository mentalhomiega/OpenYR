---
key: DominatorDamage
summary: "The damage of a psychic dominator blast."
see_also: [DominatorWarhead, "system:superweapons"]
when_omitted:
  kind: value
  value: "50"
---

The blast deals this much damage through [`DominatorWarhead`](/keys/dominatorwarhead/), credited to the firing house, before it takes units over.

```ini title="rulesmd.ini"
[General]
DominatorDamage=1000
```

The warhead's `CellSpread` sets how far the damage reaches.
