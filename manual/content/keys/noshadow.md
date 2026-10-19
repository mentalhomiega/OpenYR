---
key: NoShadow
summary: Stops a vehicle or aircraft of this type casting a voxel shadow on the ground.
see_also: [Cloakable, Crashable]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a vehicle or aircraft of this type draws no voxel shadow. Its body is drawn as usual. Infantry and structures draw no voxel shadow, so the key has no effect on them.

The rotor shadows of an aircraft with rotors are separate from its voxel shadow and are drawn as before.

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
NoShadow=yes
```
