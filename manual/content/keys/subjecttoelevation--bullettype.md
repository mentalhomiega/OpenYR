---
key: SubjectToElevation
scope: bullettype
label: Range bonus from higher ground
see_also: [ElevationIncrement, ElevationIncrementBonus, ElevationBonusCap, Arcing]
when_omitted:
  kind: value
  value: "no"
---

A weapon firing a `SubjectToElevation=yes` projectile reaches farther when its firer stands on higher ground than the target. The bonus applies only when the projectile is not [`Arcing`](/keys/arcing/) and both the firer and the target are on the ground; an object in the air, such as a flying aircraft, neither gets nor gives a bonus.

The bonus is worked out from the height difference between the firer's cell and the target's cell, in map levels. A cell carrying a bridge deck counts as four levels higher. A firer level with or below its target gets nothing.

1. Every whole [`ElevationIncrement`](/keys/elevationincrement--global-rules/) levels of difference earns [`ElevationIncrementBonus`](/keys/elevationincrementbonus--global-rules/) cells, up to [`ElevationBonusCap`](/keys/elevationbonuscap--global-rules/) cells. The result is rounded down to whole cells.
2. The bonus added to the weapon's range is the straight-line length made by those cells across and the height difference down, one level being about 0.4 cells.

So the height difference adds range even when it is too small to earn bonus cells. With the stock rulesmd.ini values (`ElevationIncrement=4`, `ElevationIncrementBonus=2`, `ElevationBonusCap=2`), a firer 2 levels up gains about 0.8 cells, and one 4 or more levels up gains 2 cells plus the height part, about 2.6 cells at 4 levels. With `ElevationIncrement=0` no weapon gets a bonus.

```ini title="rulesmd.ini"
[MYSHELL] ; example projectile
SubjectToElevation=yes
```
