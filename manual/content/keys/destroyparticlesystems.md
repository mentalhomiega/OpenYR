---
key: DestroyParticleSystems
summary: The smoke systems a structure starts when it is destroyed.
see_also: [DestroySmokeOffset, DamageParticleSystems]
when_omitted:
  kind: value
  value: ""
---

```ini title="rules.ini"
[MYBUILDING] ; a BuildingType registered in [BuildingTypes]
DestroyParticleSystems=MYSMOKESYS ; ParticleSystemTypes registered in [ParticleSystems]
DestroySmokeOffset=0,0,300
```

When the structure is destroyed, one of the listed systems starts at the structure's position plus [`DestroySmokeOffset`](/keys/destroysmokeoffset/). The system is picked at random from the entries whose [`BehavesLike`](/keys/behaveslike/) is `Smoke`; an entry with any other behavior is never used.

The system follows the structure, and it stops starting new particles when the structure is removed from the map. Particles already released finish their life.
