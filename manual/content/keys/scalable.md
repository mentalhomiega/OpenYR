---
key: Scalable
summary: Lengthens a projectile type's trail delay while many of its projectiles are in flight.
see_also: [SpawnDelay, Trailer]
when_omitted:
  kind: value
  value: "no"
---

Each time a vehicle fires a projectile of a `Scalable=yes` type, the game counts the projectiles of such types that vehicles have fired and that are still in flight, the new one included, and sets the type's trail delay from that count. The delay is the type's [`SpawnDelay`](/keys/spawndelay/#scope-bullettype) plus the number of projectiles beyond 5, divided by 3 and rounded down: 7 or fewer in flight add nothing, 8 add 1 frame, and 11 add 2. The count covers every `Scalable=yes` type together, and a projectile leaves it when it is removed.

The delay is kept on the projectile type. It decides how often every projectile of that type leaves a [`Trailer`](/keys/trailer/#scope-bullettype) puff, whoever fired it, until the next vehicle shot of that type sets it again. It does not return to `SpawnDelay` when projectiles land, and it applies from the first shot onward, so a type with no `Trailer` shows no effect.

Only a vehicle's shot counts. Infantry, aircraft and structures that fire the type do not add to the count and do not set the delay.

```ini title="rulesmd.ini"
[MYSHELL] ; a BulletType
Scalable=yes
Image=MYSHELLART
```
