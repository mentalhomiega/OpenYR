---
key: SpawnDelay
scope: bullettype
label: Projectile trail delay
see_also: ["Trailer", "Scalable", "Image"]
when_omitted:
  kind: value
  value: "3"
---

A projectile with a [`Trailer`](/keys/trailer/#scope-bullettype) drops a puff on every game frame that is a multiple of this number. A value of `0` or less disables the trail. [`Scalable=yes`](/keys/scalable/) can lengthen the delay while many projectiles of the type are in flight.

The setting is read only when the projectile's rules section sets [`Image`](/keys/image/), and it is read from the art section that `Image` names, as `Trailer` is.

```ini title="art.ini"
[MYMISSILEART]     ; the Image ID of a BulletType
Trailer=SMOKEY2    ; an AnimType registered in [Animations]
SpawnDelay=5       ; a puff every fifth frame
```
