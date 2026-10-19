---
key: RefinerySmokeFrames
summary: The frames each smoke system a harvester's payout starts at a structure lasts.
see_also: [RefinerySmokeParticleSystem, RefinerySmokeOffsetOne]
when_omitted:
  kind: value
  value: "25"
---

Each system that [`RefinerySmokeParticleSystem`](/keys/refinerysmokeparticlesystem/) starts stops emitting after this many frames. The particles it has already made finish their own life.

Use a positive value. The count ends the system when it reaches exactly `0`, so a value of `0` or less never ends the system by lifetime.

```ini title="rulesmd.ini"
[MYREFINERY] ; a BuildingType
RefinerySmokeFrames=50
```
