---
key: FallRate.ParachuteMax
scope: aircrafttype
summary: "The fastest an object of this type falls under a parachute."
see_also: ["FallRate.Parachute", ParachuteMaxFallRate]
when_omitted:
  kind: inherited
  note: The [General] ParachuteMaxFallRate value, -3 in the stock rules.
---

The limit on the downward speed, in leptons per frame, written as a negative number. A value closer to `0` makes the object drift down more slowly.

```ini title="rulesmd.ini"
[E1] ; example InfantryType
FallRate.ParachuteMax=-2
```
