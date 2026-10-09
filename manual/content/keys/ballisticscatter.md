---
key: BallisticScatter
summary: How far an inaccurate arcing shot has its aim point thrown off.
see_also: ["Inaccurate", "Arcing", "HomingScatter"]
when_omitted:
  kind: value
  value: "1"
---

An inaccurate arcing shot has its aim point moved by up to this many cells. The scatter applies only to a projectile type that sets both [`Inaccurate=yes`](/keys/inaccurate/) and [`Arcing=yes`](/keys/arcing/). A type with only one of the two flags is not scattered.

Each shot's aim point moves in a random direction by a random distance between half this value and the full value. A shell with [`FlakScatter=yes`](/keys/flakscatter/) is scattered by its own rule instead: its distance grows with the distance to the target, and an invisible one draws up to twice this value.

The aim point moves before the launch is worked out, so the shot's pitch and speed are worked out for the moved point. A projectile type with [`ROT=0`](/keys/rot/#scope-bullettype) that is not [`Dropping=yes`](/keys/dropping/) also leaves toward the moved point, so it comes down near it. A type with a nonzero `ROT` or `Dropping=yes` leaves in the direction the firer faces. [`Inaccurate`](/keys/inaccurate/) covers the other effects of that flag.
