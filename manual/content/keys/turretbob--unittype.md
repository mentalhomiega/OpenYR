---
key: TurretBob
scope: unittype
label: Turret and barrel bob while the hull walks
see_also: [TurretRecoil, Turret, Voxel, WalkRate]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the turret and barrel of a [voxel](/keys/voxel/) vehicle shift up by the hull's frame number. Frames 0, 7 and 15 leave the turret in place, frames 1, 6, 8 and 14 move it up one pixel, and every other frame moves it up two pixels. The frame follows the walk counter, so the shift changes as the counter steps. The hull itself does not move.

With `no`, the turret and barrel stay on the hull's pixel on every frame, as Yuri's Revenge draws them.

The key has no effect on a vehicle without a turret, or on a vehicle drawn from shape artwork.

```ini title="rulesmd.ini"
[MYTANK] ; a UnitType with Voxel=yes and a turret
TurretBob=yes ; the turret and barrel bob with the hull's walk frames
```
