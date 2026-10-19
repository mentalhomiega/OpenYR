---
key: RefinerySmokeOffsetOne
summary: Where the first smoke system of a harvester's payout starts, relative to the structure.
see_also: [RefinerySmokeParticleSystem, RefinerySmokeOffsetTwo, RefinerySmokeOffsetThree, RefinerySmokeOffsetFour, DamageSmokeOffset]
when_omitted:
  kind: value
  value: 0,0,0
---

The offset is X, Y and Z in leptons, 256 to a cell, added to the structure's position, which is the center of its reference cell at ground height. [`RefinerySmokeParticleSystem`](/keys/refinerysmokeparticlesystem/) starts one system at the resulting point on each payout.

`RefinerySmokeOffsetOne` to `RefinerySmokeOffsetFour` each place one system, and a payout starts only the ones that are set. An offset of `0,0,0` is not set, so the omitted value starts no system. An offset of `128,128,0` is not set either.

:::note[Write all three numbers]
A value with fewer than three numbers, such as `0,90`, is malformed and ignored. [INI syntax](/formats/ini-syntax/#malformed-values) has the rule.
:::

```ini title="rulesmd.ini"
[MYREFINERY] ; a BuildingType
RefinerySmokeOffsetOne=-85,-85,220
RefinerySmokeOffsetTwo=-85,90,220
```
