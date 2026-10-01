---
key: AmerParaDropInf
summary: "The infantry types an American paradrop brings, one cargo plane for each."
see_also: [AmerParaDropNum, ParadropRadius, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A [`Type=AmerParaDrop`](/keys/type/#scope-superweapontype) weapon drops these infantry types, whatever the firing house's side. Each entry sends one cargo plane, which carries as many infantry of that type as the entry at the same position in [`AmerParaDropNum`](/keys/amerparadropnum/). [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
AmerParaDropInf=E1
AmerParaDropNum=8
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
