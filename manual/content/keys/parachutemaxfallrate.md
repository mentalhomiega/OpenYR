---
key: ParachuteMaxFallRate
summary: "The fastest an object falls under a parachute, in leptons a frame."
see_also: [NoParachuteMaxFallRate, Parachute]
when_omitted:
  kind: value
  value: "-3"
---

An object falling under a parachute, such as a paratrooper, speeds up by 1 lepton a frame each frame until it falls this fast. The value is negative because it is a downward speed.

```ini title="rulesmd.ini"
[General]
ParachuteMaxFallRate=-3
```
