---
key: DestroySmokeOffset
summary: Where a destroyed structure's smoke system starts, relative to the structure.
see_also: [DestroyParticleSystems, DamageSmokeOffset]
when_omitted:
  kind: value
  value: 0,0,0
---

```ini title="rules.ini"
[MYBUILDING] ; a BuildingType registered in [BuildingTypes]
DestroyParticleSystems=MYSMOKESYS ; ParticleSystemTypes registered in [ParticleSystems]
DestroySmokeOffset=0,0,300 ; the smoke starts 300 leptons above the structure's position
```

The offset is three lepton distances, X, Y and Z, added to the structure's position when one of its [`DestroyParticleSystems`](/keys/destroyparticlesystems/) starts. X and Y are map directions, so the offset does not turn with the structure.
