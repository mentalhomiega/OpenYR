---
key: SovParaDropNum
summary: "How many infantry each a Soviet paradrop plane carries."
see_also: [SovParaDropInf, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each entry is the number of paratroopers the cargo plane for the [`SovParaDropInf`](/keys/sovparadropinf/) entry at the same position carries. [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
SovParaDropInf=E2
SovParaDropNum=9
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
