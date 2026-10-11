---
key: CourseLockDuration
summary: Keeps a homing projectile on its launch course for this many frames, whatever its speed.
see_also: [ROT, Acceleration, "system:projectile-flight"]
when_omitted:
  kind: value
  value: "0"
---

A homing projectile, one with a [`ROT`](/keys/rot/#scope-bullettype) above `0`, flies straight out of its launcher while its course is locked. It does not steer toward its target during that time. A projectile with `CourseLockDuration=0` stays locked until it reaches its weapon's speed, and one whose weapon has a `Speed` of 16 or more is never locked. With a value above `0`, the lock lasts that many frames after launch, whatever the projectile's speed, and the projectile gains its full [`Acceleration`](/keys/acceleration/#scope-bullettype) each frame meanwhile.

The key has no effect on a projectile with no rate of turn.

Stock rules set `CourseLockDuration=15` on `MedusaProjectile`, the Aegis cruiser's missile. It flies 15 frames on the direction it was fired along, which is why the fan of an object with [`RadialFireSegments`](/keys/radialfiresegments/) spreads before the missiles curve onto the target.

```ini title="rules.ini"
[MYMISSILE] ; a BulletType registered in [Projectiles]
ROT=20
CourseLockDuration=15 ; flies straight for 15 frames
```
