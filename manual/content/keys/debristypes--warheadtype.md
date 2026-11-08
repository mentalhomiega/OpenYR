---
key: DebrisTypes
scope: warheadtype
label: Voxel debris from a shot
see_also: [MaxDebris, DebrisMaximums, MetallicDebris]
when_omitted:
  kind: value
  value: ""
---

On a warhead, `DebrisTypes` lists the voxel pieces a shot throws where it detonates. The list is used only when [`MaxDebris`](/keys/maxdebris/#scope-warheadtype) is above `0`. With a list set, the shot throws no [`MetallicDebris`](/keys/metallicdebris/) animations; with the list empty, it throws those instead.

Entries are thrown in list order, each up to its [`DebrisMaximums`](/keys/debrismaximums/#scope-warheadtype) figure. The list is cycled until the count is used up. An entry with no figure, or a figure of `0` or less, throws none. If no entry has a figure above `0`, the shot throws no voxel pieces and no metallic ones.

`DebrisTypes=none` empties the list.

```ini title="rulesmd.ini"
[MYSHELL] ; a Warhead
MinDebris=2
MaxDebris=4
DebrisTypes=MYSCRAP,MYTIRE ; VoxelAnimTypes registered in [VoxelAnims]
DebrisMaximums=2,1
```
