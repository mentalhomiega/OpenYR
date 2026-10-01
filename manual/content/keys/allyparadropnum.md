---
key: AllyParaDropNum
summary: "How many infantry each an Allied paradrop plane carries."
see_also: [AllyParaDropInf, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each entry is the number of paratroopers the cargo plane for the [`AllyParaDropInf`](/keys/allyparadropinf/) entry at the same position carries. [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
AllyParaDropInf=E1,GGI
AllyParaDropNum=6,2
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
