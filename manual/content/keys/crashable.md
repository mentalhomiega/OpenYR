---
key: Crashable
summary: Whether a destroyed object of this type falls, or what happens to its passengers when it is destroyed.
see_also: [CrashingSound, "system:emp-pulse", "system:destruction-and-debris", "system:transports"]
---

On an aircraft type, `Crashable` decides whether the aircraft falls when it is destroyed in the air. The [aircraft page](/keys/crashable/#scope-aircrafttype) gives its default and details.

On a vehicle type, `Crashable` decides what happens to the vehicle's passengers. Omitted or `no`, each passenger that can enter the vehicle's cell gets out, as [transports](/systems/transports/#losing-the-transport) describes. With `Crashable=yes`, the passengers die with the vehicle. A vehicle with a jumpjet locomotor that is destroyed in the air then falls spinning until it lands, where it explodes and is removed; any other vehicle is removed where it dies. A vehicle destroyed more than 208 leptons above the ground credits its attacker with those kills, and a lower one credits no attacker.

On an infantry type with a jumpjet locomotor, such as the Rocketeer, `Crashable=yes` makes a soldier destroyed in the air fall instead of vanishing. He plays his air death sequences while he drops, then lands, plays `AirDeathFinish` and is removed. A soldier destroyed on the ground, and one of any other infantry type, dies as before.

A structure type reads the key and ignores it.

```ini title="rulesmd.ini"
[MYJUMPJET] ; example UnitType with a jumpjet locomotor
Crashable=yes
```
