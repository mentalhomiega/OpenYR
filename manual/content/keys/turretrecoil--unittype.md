---
key: TurretRecoil
scope: unittype
label: Vehicle turret recoil
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a vehicle's turret jerks back for one frame each time the vehicle fires, and its barrel moves back with it. Without it, a vehicle's turret does not move when it fires.

The jerk is a fixed offset. It does not use [`TurretTravel`](/keys/turrettravel/), [`BarrelTravel`](/keys/barreltravel/) or the compress, hold and recover frame keys, which apply to structures only.
