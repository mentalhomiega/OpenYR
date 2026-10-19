---
key: FallRate.NoParachute
scope: aircrafttype
summary: "How fast an object of this type, falling without a parachute, speeds up."
see_also: ["FallRate.NoParachuteMax", "FallRate.Parachute", NoParachuteMaxFallRate]
when_omitted:
  kind: value
  value: "the stock rate of about 1.4 leptons per frame"
---

While an object of this type falls with no parachute, its downward speed grows by this many leptons per frame until it reaches [`FallRate.NoParachuteMax`](/keys/fallrate.noparachutemax/).

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
FallRate.NoParachute=2
```
