---
key: MaxDebris
summary: The number of pieces of wreckage a destroyed object may throw.
see_also: [DebrisTypes, DebrisMaximums, MetallicDebris, ExplosiveVoxelDebris]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
MaxDebris=6
DebrisTypes=MYSCRAP,MYTIRE ; VoxelAnimTypes registered in [VoxelAnims]
DebrisMaximums=4,2
```

At `0`, a destroyed object throws no wreckage, whatever its debris lists say. Above `0`, the object throws a random number of pieces from [`MinDebris`](/keys/mindebris/) up to one less than `MaxDebris`. Which pieces are thrown depends on the type's lists:

- **With [`DebrisTypes`](/keys/debristypes/)**, the entries are taken in turn, starting again from the first after the last. Each turn throws a random number of pieces, from zero up to the entry's matching [`DebrisMaximums`](/keys/debrismaximums/) figure, until the count is used up. An entry with no matching figure throws none. When two whole passes throw nothing, the rest of the count goes unused. The pieces start at the object's center and belong to the object's owner.
- **With [`DebrisAnims`](/keys/debrisanims/)**, whatever count the voxel pieces left over is thrown as those animations, each picked at random, 20 leptons above the object's center.
- **With neither list**, the whole count is thrown as [`MetallicDebris`](/keys/metallicdebris/) animations, picked the same way. If `MetallicDebris` is empty, nothing is thrown.

In the example, the tank throws up to five pieces: each turn `MYSCRAP` throws zero to four and `MYTIRE` zero to two, until the count is used up.

Wreckage is thrown after the death voice and before any [`Explodes=yes`](/keys/explodes/#scope-aircrafttype) blast. An object that has fallen from a height, for example off a bridge, and dies over water within 10 leptons of the ground throws no wreckage and does not explode.
