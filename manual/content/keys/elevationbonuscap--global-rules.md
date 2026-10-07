---
key: ElevationBonusCap
scope: global-rules
label: Most cells of elevation range bonus
see_also: [ElevationIncrement, ElevationIncrementBonus, SubjectToElevation]
when_omitted:
  kind: value
  value: "0.0"
---

`[ElevationModel]` `ElevationBonusCap` limits the cells of range a firer earns from [`ElevationIncrementBonus`](/keys/elevationincrementbonus/) steps. The height difference itself still adds range above the cap, as [SubjectToElevation](/keys/subjecttoelevation/) describes. When the key is omitted the cap is `0`, so only the height difference adds range.

```ini title="rulesmd.ini"
[ElevationModel]
ElevationBonusCap=2 ; at most 2 cells from elevation steps
```
