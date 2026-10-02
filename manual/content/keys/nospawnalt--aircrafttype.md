---
key: NoSpawnAlt
scope: aircrafttype
label: 'Empty launcher model'
see_also: [Spawns, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a vehicle that carries [`Spawns`](/keys/spawns/#scope-aircrafttype) draws its body from the voxel model `<image>WO.VXL`, with motion data from `<image>WO.HVA`, while none of its spawns is docked. The V3 launcher uses this to show an empty rail after its rocket leaves. Without the `WO` files, or while a spawn is docked, the normal model is drawn. Only vehicles drawn as voxels use the key; its turret and shadow are unchanged.

```ini title="rulesmd.ini"
[MYLAUNCHER] ; example VehicleType
Spawns=MYROCKET
SpawnsNumber=1
NoSpawnAlt=yes
```
