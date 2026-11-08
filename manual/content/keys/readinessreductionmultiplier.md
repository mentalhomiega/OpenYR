---
key: ReadinessReductionMultiplier
summary: Scales how much ammunition a DamageReducesReadiness object loses to each hit.
see_also: [DamageReducesReadiness, Ammo]
when_omitted:
  kind: value
  value: "0"
---

`ReadinessReductionMultiplier` scales the ammunition a [`DamageReducesReadiness=yes`](/keys/damagereducesreadiness/) object loses. A hit removes the full [`Ammo`](/keys/ammo/) load times this value times the hit's share of full strength, so `1` empties the object as its strength runs out and `2` empties it by half strength.

```ini title="rules.ini"
[MyRocketTruck] ; example VehicleType
DamageReducesReadiness=yes
ReadinessReductionMultiplier=1.5
```

At the default `0`, hits cost no ammunition. The setting has no effect without `DamageReducesReadiness=yes`.
