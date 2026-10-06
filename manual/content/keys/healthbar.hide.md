---
key: HealthBar.Hide
summary: Stops the health bar from being drawn over objects of this type.
see_also: [PixelSelectionBracketDelta, ConditionRed, ConditionYellow]
when_omitted:
  kind: value
  value: "no"
---

With `HealthBar.Hide=yes`, no health bar appears over an object of this type, whether the object is selected or the cursor points at it. The selection brackets, cargo pips and rank insignia are drawn as before.

The key applies to infantry, vehicles, aircraft and structures.

```ini title="rulesmd.ini"
[CAMOBLDG] ; example BuildingType whose condition stays hidden
HealthBar.Hide=yes
```
