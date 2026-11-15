---
key: SubjectToTrenches
summary: "With no, every shot of this projectile at a structure with UC.PassThrough passes through to its occupants."
see_also: [UC.PassThrough, "system:garrisons"]
when_omitted:
  kind: value
  value: "yes"
---

A projectile with `SubjectToTrenches=no` passes every shot into an occupied structure whose [`UC.PassThrough`](/keys/uc.passthrough/) is above `0%`. The name is misleading, since the key affects every structure with a pass-through, not only trenches. With the default, the structure's own `UC.PassThrough` chance applies. Damage that comes from no projectile, such as a demolition charge, is always subject to the structure's chance.

```ini title="rulesmd.ini"
[MyBullet] ; example BulletType
SubjectToTrenches=no
```
