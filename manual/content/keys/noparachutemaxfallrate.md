---
key: NoParachuteMaxFallRate
summary: "The fastest an object falls without a parachute, in leptons a frame."
see_also: [ParachuteMaxFallRate, Parachute]
when_omitted:
  kind: value
  value: "-100"
---

An object falling without a parachute speeds up by gravity each frame until it falls this fast. The value is negative because it is a downward speed.

```ini title="rulesmd.ini"
[General]
NoParachuteMaxFallRate=-100
```
