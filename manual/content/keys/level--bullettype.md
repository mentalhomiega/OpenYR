---
key: Level
scope: bullettype
label: Level homing flight
see_also: [ROT, Speed, Airburst]
when_omitted:
  kind: value
  value: "no"
---

`Level=yes` makes a homing projectile keep the pitch it was fired with while it has a target. It does not climb over terrain or turn its pitch toward the target, but it still turns sideways toward its target. A projectile with no target is not affected.

Only a homing projectile reads the setting: one whose [`ROT`](/keys/rot/#scope-bullettype) is above zero. A projectile with no rate of turn follows its launch arc, and this setting has no effect on it.
