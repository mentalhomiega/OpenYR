---
key: RMGMaximumTiberium
summary: How much tiberium each player adds to a generated map's fields at the highest tiberium setting.
see_also: [RMGMinimumTiberium, "system:map-generation"]
when_omitted:
  kind: value
  value: "5500"
---

`RMGMaximumTiberium` is how much tiberium each player adds to a generated map's layout fields at [`Tiberium=100`](/keys/tiberium/#scope-random-map-generation). Each unit covers one bare cell or thickens a covered cell by one stage. [`RMGMinimumTiberium`](/keys/rmgminimumtiberium/) gives the amount at `Tiberium=0` and explains how a setting between them is worked out.

```ini title="RMGMD.INI"
[General]
RMGMaximumTiberium=1050
```
