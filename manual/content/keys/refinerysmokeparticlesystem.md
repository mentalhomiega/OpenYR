---
key: RefinerySmokeParticleSystem
summary: The particle system a structure starts each time a harvester pays out at it.
see_also: [RefinerySmokeOffsetOne, RefinerySmokeFrames, HarvesterDumpRate]
when_omitted:
  kind: value
  value: none
---

A docked harvester or weeder takes a pass every [`HarvesterDumpRate`](/keys/harvesterdumprate/) minutes. On each pass, the structure west of the harvester starts one copy of this particle system at each of its [`RefinerySmokeOffsetOne`](/keys/refinerysmokeoffsetone/) to `RefinerySmokeOffsetFour` positions that is set. This includes the last pass of a dock, which finds the harvester empty. A structure with no system named starts nothing.

Each copy lasts [`RefinerySmokeFrames`](/keys/refinerysmokeframes/) frames, so the copies from successive passes overlap.

```ini title="rulesmd.ini"
[MYREFINERY] ; a BuildingType registered in [BuildingTypes]
RefinerySmokeParticleSystem=SmallGreySSys ; a ParticleSystemType registered in [ParticleSystems]
RefinerySmokeOffsetOne=-92,-208,312
RefinerySmokeOffsetTwo=-92,208,312
RefinerySmokeFrames=50
```
