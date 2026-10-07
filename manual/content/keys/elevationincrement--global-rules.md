---
key: ElevationIncrement
scope: global-rules
label: Levels per elevation range step
see_also: [ElevationIncrementBonus, ElevationBonusCap, SubjectToElevation]
when_omitted:
  kind: value
  value: "0"
---

`[ElevationModel]` `ElevationIncrement` sets how many map levels a firer must stand above its target to earn one step of [elevation range bonus](/keys/subjecttoelevation/). Only weapons whose projectile sets `SubjectToElevation=yes` are affected. With `0`, no weapon gets an elevation bonus at all.

```ini title="rulesmd.ini"
[ElevationModel]
ElevationIncrement=4 ; one step for every 4 levels of height difference
```
