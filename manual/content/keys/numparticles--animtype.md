---
key: NumParticles
scope: animtype
label: Particles per largest frame
see_also: ["SpawnsParticle", "Crater"]
when_omitted:
  kind: value
  value: "0"
---

The number of particles that [`SpawnsParticle`](/keys/spawnsparticle/#scope-animtype) puts out each time the animation reaches its largest frame. A value of zero or less spawns none, and the key does nothing without a `SpawnsParticle`.
