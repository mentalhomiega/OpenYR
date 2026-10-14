---
key: Crashable
summary: Whether a destroyed object of this type falls, or what happens to its passengers when it is destroyed.
see_also: [CrashingSound, "system:emp-pulse", "system:destruction-and-debris", "system:transports"]
---

On an aircraft type, `Crashable` decides whether the aircraft falls when it is destroyed in the air. The [aircraft page](/keys/crashable/#scope-aircrafttype) gives its default and details.

On a vehicle type, `Crashable` decides what happens to the vehicle's passengers. Omitted or `no`, each passenger that can enter the vehicle's cell gets out, as [transports](/systems/transports/#losing-the-transport) describes. With `Crashable=yes`, the passengers die with the vehicle, and the vehicle is taken off the map where it dies. A vehicle destroyed more than 208 leptons above the ground credits its attacker with those kills, and a lower one credits no attacker.

An infantry or structure type reads the key and ignores it.

```ini title="rulesmd.ini"
[MYJUMPJET] ; example UnitType with a jumpjet locomotor
Crashable=yes
```
