---
key: DominatorWarhead
summary: "The warhead of a psychic dominator blast."
see_also: [DominatorDamage, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

The psychic dominator blast deals [`DominatorDamage`](/keys/dominatordamage/) through this warhead.

```ini title="rulesmd.ini"
[General]
DominatorWarhead=MyDominatorWH ; a WarheadType registered in [Warheads]
```

With no warhead, the blast does no damage, but still takes units over.
