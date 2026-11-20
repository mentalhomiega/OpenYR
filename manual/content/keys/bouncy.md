---
key: Bouncy
summary: Read for a projectile type, but changes nothing; a ballistic projectile never rebounds.
see_also: [Elasticity, Arcing]
when_omitted:
  kind: value
  value: "no"
---

The setting changes nothing. A ballistic projectile, one whose [`ROT`](/keys/rot/#scope-bullettype) is zero, never rebounds. While it still knows its firer it flies on below the ground it lands on, and once the firer is forgotten it detonates on its first landing. [Ballistic flight](/systems/projectile-flight/#ballistic-flight) gives the full landing rules.
