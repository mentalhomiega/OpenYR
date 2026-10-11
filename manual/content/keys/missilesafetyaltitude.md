---
key: MissileSafetyAltitude
summary: "How high a homing projectile with no target left must be before it detonates."
see_also: ["system:projectile-flight", MissileROTVar]
when_omitted:
  kind: value
  value: "500"
---

```ini title="rulesmd.ini"
[General]
MissileSafetyAltitude=750
```

A [homing](/keys/rot/#scope-bullettype) projectile that has lost its target detonates once it is at least this many leptons above the ground. A projectile moving at 2 leptons a frame or more [already goes off at once](/systems/projectile-flight/#steered-flight) where it is, so the value matters only to a slower one.
