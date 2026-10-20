---
key: DebrisMaximums
scope: warheadtype
label: Voxel debris caps from a shot
see_also: [DebrisTypes, MaxDebris]
when_omitted:
  kind: value
  value: ""
---

On a warhead, each figure caps the voxel type in the same position of [`DebrisTypes`](/keys/debristypes/#scope-warheadtype). Each step through the list draws a random count from zero up to that figure, and the count is cut short when the shot's piece count runs out. A type with no figure, or a figure of `0` or less, throws none of its pieces.

```ini title="rulesmd.ini"
[MYSHELL] ; a Warhead
MaxDebris=4
DebrisTypes=MYSCRAP,MYTIRE ; VoxelAnimTypes registered in [VoxelAnims]
DebrisMaximums=3,2 ; at most three MYSCRAP, then at most two MYTIRE
```
