---
key: DamageAirThreshold
summary: How far above the ground a blast of this warhead must be to damage objects in the air.
see_also: [CellSpread, "system:warheads"]
when_omitted:
  kind: value
  value: "0"
---

A blast of this warhead damages an object in the air within its [`CellSpread`](/keys/cellspread/) only when the blast is more than `DamageAirThreshold` leptons above the ground under it. The stock `0` means any height above the ground. `-1` lets a blast on the ground reach objects in the air, and a large value such as `9999` keeps an airborne blast from reaching them. The distance to the object still has to be within the spread, and a flying object is 750 leptons or more above the ground, so a ground blast needs a spread of 3 cells or more.

```ini title="rulesmd.ini"
[MYFLAK] ; example WarheadType
CellSpread=4
DamageAirThreshold=-1
```
