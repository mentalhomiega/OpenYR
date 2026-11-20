---
key: Elasticity
scope: bullettype
label: Projectile rebound damping
see_also: [Bouncy, Arcing]
when_omitted:
  kind: value
  value: ".75"
---

The setting changes nothing. A ballistic projectile never rebounds: [`Bouncy`](/keys/bouncy/) is read but has no effect, and [Ballistic flight](/systems/projectile-flight/#ballistic-flight) describes what a landing does.
