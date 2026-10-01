---
key: AllyParaDropInf
summary: "The infantry types an Allied paradrop brings, one cargo plane for each."
see_also: [AllyParaDropNum, ParadropRadius, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A [`Type=ParaDrop`](/keys/type/#scope-superweapontype) weapon fired by a house of the first side in `[Sides]` drops these infantry types. The list also sets how many planes a [`Type=SpyPlane`](/keys/type/#scope-superweapontype) weapon sends, one for each entry. Each entry sends one cargo plane, which carries as many infantry of that type as the entry at the same position in [`AllyParaDropNum`](/keys/allyparadropnum/). [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
AllyParaDropInf=E1,GGI
AllyParaDropNum=6,2
```

Keep the two lists the same length. When they differ, the shot spends its charge and no plane comes.
