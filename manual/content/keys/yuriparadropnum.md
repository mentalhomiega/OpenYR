---
key: YuriParaDropNum
summary: "How many infantry each a Yuri paradrop plane carries."
see_also: [YuriParaDropInf, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Each entry is the number of paratroopers the cargo plane for the [`YuriParaDropInf`](/keys/yuriparadropinf/) entry at the same position carries. [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
YuriParaDropInf=INIT
YuriParaDropNum=6
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
