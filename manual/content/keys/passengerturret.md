---
key: PassengerTurret
summary: "With yes, a multi-turret vehicle shows the turret numbered by how many passengers it carries."
see_also: [TurretCount, Passengers, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "no"
---

`PassengerTurret` is an Ares key for a vehicle type with [`TurretCount`](/keys/turretcount/) above `0`. With `yes`, the vehicle shows turret 0 while empty, turret 1 with one passenger, and so on: with a graphic named `FOO`, those are `FOOTUR`, `FOOTUR1` and onward. A vehicle carrying as many passengers as it has turrets, or more, shows its last turret, numbered one below `TurretCount`. A gattling type ignores the key.

Only the turret changes. The vehicle keeps firing the weapon it would fire without the key, so a type that fires one weapon whatever it carries can set `WeaponCount=1`. The count is the number of passengers aboard, whatever their size.

```ini title="rulesmd.ini"
[MYPRISONTRUCK] ; example VehicleType
Turret=yes
TurretCount=4
WeaponCount=1
Weapon1=MyPrisonBeam
Passengers=3
PassengerTurret=yes
```
