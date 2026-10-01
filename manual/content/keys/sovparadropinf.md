---
key: SovParaDropInf
summary: "The infantry types a Soviet paradrop brings, one cargo plane for each."
see_also: [SovParaDropNum, ParadropRadius, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A [`Type=ParaDrop`](/keys/type/#scope-superweapontype) weapon fired by a house of any side other than the first and third in `[Sides]` drops these infantry types. Each entry sends one cargo plane, which carries as many infantry of that type as the entry at the same position in [`SovParaDropNum`](/keys/sovparadropnum/). [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
SovParaDropInf=E2
SovParaDropNum=9
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
