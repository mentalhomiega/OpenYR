---
key: Gattling.Cycle
summary: With yes, a gattling weapon that finishes its last stage starts again at the first.
see_also: [IsGattling, WeaponStages, RateUp, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "no"
---

A `Gattling.Cycle=yes` type that has spun up to its last stage and has reached that stage's threshold goes back to its first stage with no spin and keeps its target. It then spins up through the stages again, so a long engagement alternates between the weapons of each stage. Without the key, the weapon stays at its last stage while it fires and drops a stage only when it stops.

The key applies to a gattling type with two or more [`WeaponStages`](/keys/weaponstages/).

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=3
Gattling.Cycle=yes
```
