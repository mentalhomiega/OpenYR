---
key: YuriParaDropInf
summary: "The infantry types a Yuri paradrop brings, one cargo plane for each."
see_also: [YuriParaDropNum, ParadropRadius, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A [`Type=ParaDrop`](/keys/type/#scope-superweapontype) weapon fired by a house of the third side in `[Sides]` drops these infantry types. Each entry sends one cargo plane, which carries as many infantry of that type as the entry at the same position in [`YuriParaDropNum`](/keys/yuriparadropnum/). [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
YuriParaDropInf=INIT
YuriParaDropNum=6
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
