---
key: DirectRockingCoefficient
scope: global-rules
label: Direct rocking coefficient
summary: Scales the rocking force of direct rocker blasts.
see_also: [DirectRocker]
when_omitted:
  kind: value
  value: "1"
---

The force that a [`DirectRocker`](/keys/directrocker/) blast gives a vehicle is its damage multiplied by this value, divided by 100, and capped at 4. At `1` the force equals the force a [`Rocker`](/keys/rocker/) blast of the same damage gives. `0` stops direct rocker blasts from rocking vehicles.

Set it in the `[AudioVisual]` section.

```ini title="rules.ini"
[AudioVisual]
DirectRockingCoefficient=1
```
