---
key: DetonationAltitude
summary: "The height at which a vertical projectile explodes."
see_also: [Vertical, NukeMaker]
when_omitted:
  kind: value
  value: "0"
---

A [`Vertical=yes`](/keys/vertical/) projectile explodes once it climbs above this height, in leptons. A projectile that explodes with a [`NukeMaker=yes`](/keys/nukemaker/) warhead also drops its payload from this height above the target.

```ini title="rulesmd.ini"
[MyNukeUp] ; example projectile
Vertical=yes
DetonationAltitude=20000
```

The key has no effect on a projectile that is not vertical.
