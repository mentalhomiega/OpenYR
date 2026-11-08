---
key: DamageReducesReadiness
summary: Makes each hit on the object cost it ammunition in proportion to the damage.
see_also: [ReadinessReductionMultiplier, Ammo, Reload, EmptyReload]
when_omitted:
  kind: value
  value: "no"
---

With `DamageReducesReadiness=yes`, every hit takes ammunition away from the object as well as strength. The ammunition lost is the full [`Ammo`](/keys/ammo/) load times [`ReadinessReductionMultiplier`](/keys/readinessreductionmultiplier/) times the hit's share of full [`Strength`](/keys/strength/#scope-aircrafttype). The result is rounded toward zero, and the object never drops below no ammunition.

```ini title="rules.ini"
[MyRocketTruck] ; example VehicleType
Strength=300
Ammo=10
DamageReducesReadiness=yes
ReadinessReductionMultiplier=2 ; a 30-point hit costs 10 x 2 x 30/300 = 2 rounds
```

The hit is counted after the owner's armor bonuses and before the warhead's [`Verses`](/keys/verses/) values, so a hit that `Verses` reduces to nothing still costs ammunition. A hit that the [Iron Curtain](/systems/superweapons/#iron-curtain), a warp-out or an immunity turns away costs nothing. Healing works the other way: it adds ammunition by the same formula, and may go past the full load.

After every counted hit, a vehicle restarts its reload countdown, using [`EmptyReload`](/keys/emptyreload/) when it has no ammunition left and [`Reload`](/keys/reload/) otherwise. Other objects only lose the ammunition.

The default [`ReadinessReductionMultiplier`](/keys/readinessreductionmultiplier/) is `0`, so this setting does nothing until the multiplier is also set. Stock Yuri's Revenge uses it on no object.
