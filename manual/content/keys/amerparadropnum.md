---
key: AmerParaDropNum
summary: "How many infantry each an American paradrop plane carries."
see_also: [AmerParaDropInf, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each entry is the number of paratroopers the cargo plane for the [`AmerParaDropInf`](/keys/amerparadropinf/) entry at the same position carries. [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
AmerParaDropInf=E1
AmerParaDropNum=8
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
