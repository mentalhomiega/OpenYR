---
key: ProjectileRange
summary: How far a Ranged=yes projectile flies before it detonates, measured from where it was fired.
see_also: ["Range", "Ranged", "Projectile"]
when_omitted:
  kind: value
  value: "390.625"
---

An Ares key. A projectile with [`Ranged=yes`](/keys/ranged/) detonates in the air once it has flown this far across the ground from the point it was fired, in cells. The distance is horizontal; height does not count. Without a `Ranged=yes` projectile the key has no effect. To set how far a weapon fires, use [`Range=`](/keys/range/).

```ini title="rulesmd.ini"
[MYGUN] ; example WeaponType whose projectile has Ranged=yes
Range=8
ProjectileRange=3  ; the shell bursts after 3 cells
```
