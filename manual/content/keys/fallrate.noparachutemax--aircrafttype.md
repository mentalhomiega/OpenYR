---
key: FallRate.NoParachuteMax
scope: aircrafttype
summary: "The fastest an object of this type falls with no parachute."
see_also: ["FallRate.NoParachute", NoParachuteMaxFallRate]
when_omitted:
  kind: inherited
  note: The [General] NoParachuteMaxFallRate value, -100 in the stock rules.
---

The limit on the downward speed, in leptons per frame, written as a negative number. With `-3` an object dropped from the air falls as slowly as one under a parachute.

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
FallRate.NoParachuteMax=-3
```
