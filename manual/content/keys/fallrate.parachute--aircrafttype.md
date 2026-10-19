---
key: FallRate.Parachute
scope: aircrafttype
summary: "How fast an object of this type, parachuting, speeds up its fall."
see_also: ["FallRate.ParachuteMax", "FallRate.NoParachute", ParachuteMaxFallRate]
when_omitted:
  kind: value
  value: "1 lepton per frame, as the stock game falls with a parachute"
---

While an object of this type falls under a parachute, its downward speed grows by this many leptons per frame until it reaches [`FallRate.ParachuteMax`](/keys/fallrate.parachutemax/).

```ini title="rulesmd.ini"
[E1] ; example InfantryType
FallRate.Parachute=1
```
