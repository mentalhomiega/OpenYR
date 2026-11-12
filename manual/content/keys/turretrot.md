---
key: TurretROT
summary: "The turret's own rate of turn, separate from ROT."
see_also: ["ROT"]
when_omitted:
  kind: inherited
  note: The type's ROT.
---

An Ares key. `TurretROT=` sets how fast the turret of a vehicle turns, in 1/256ths of a circle per frame, from 0 to 127. A turret turns at the type's [`ROT`](/keys/rot/) when the key is absent. A unit may fire only once its turret faces the target, so the key also changes how soon it fires after a turn. A limpet drain slows the turret to the same fraction of `TurretROT` that it slows the body to of `ROT`.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
ROT=5
TurretROT=12 ; turret turns faster than the hull
```
