---
key: DebrisAnims
summary: The flat wreckage animations the object throws when destroyed.
see_also: [MaxDebris, MinDebris, DebrisTypes, MetallicDebris]
when_omitted:
  kind: value
  value: none
---

The value lists AnimTypes. A destroyed object throws whatever part of its debris count its voxel [`DebrisTypes`](/keys/debristypes/) did not use as these animations, each picked at random and placed 20 leptons above the object's center. [`MaxDebris`](/keys/maxdebris/) sets the count. A type with this list never falls back to [`MetallicDebris`](/keys/metallicdebris/).

```ini title="rulesmd.ini"
[MyBuilding] ; example BuildingType
MaxDebris=4
DebrisAnims=MYDBRIS1,MYDBRIS2 ; AnimTypes registered in [Animations]
```
