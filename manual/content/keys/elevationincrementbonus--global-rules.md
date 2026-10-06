---
key: ElevationIncrementBonus
scope: global-rules
label: Cells per elevation range step
see_also: [ElevationIncrement, ElevationBonusCap, SubjectToElevation]
when_omitted:
  kind: value
  value: "1.0"
---

`[ElevationModel]` `ElevationIncrementBonus` sets how many cells of range each [`ElevationIncrement`](/keys/elevationincrement--global-rules/) step of height difference is worth. The total is limited by [`ElevationBonusCap`](/keys/elevationbonuscap--global-rules/) and rounded down to whole cells; [SubjectToElevation](/keys/subjecttoelevation--bullettype/) explains how it combines with the height difference.

```ini title="rulesmd.ini"
[ElevationModel]
ElevationIncrementBonus=2 ; 2 cells of range per step
```
