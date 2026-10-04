---
key: PixelSelectionBracketDelta
summary: Moves a unit's selection border and health bar up or down by this many pixels.
see_also: [ConditionYellow, ConditionRed]
when_omitted:
  kind: value
  value: "0"
---

A selected vehicle, aircraft or infantry soldier shows a bordered health bar above its centre. `PixelSelectionBracketDelta` shifts both the border and the bar vertically by this many screen pixels. Negative values raise them, which suits tall artwork; positive values lower them. Structures ignore the key, because their health bar follows the structure's [`Height`](/keys/height/#scope-buildingtype).

```ini title="rulesmd.ini"
[MyWalker] ; example VehicleType with tall artwork
PixelSelectionBracketDelta=-12
```
