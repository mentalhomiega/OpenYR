---
key: AroundTarget
summary: "Whether the bomblets of a splitting or spreading projectile are placed around its target or around the point it burst at."
see_also: [AirburstSpread, Splits, RetargetAccuracy, "system:projectile-flight"]
when_omitted:
  kind: context-dependent
  note: "Follows `Splits`, so a splitting projectile looks around its target and a spreading one around the point it burst at."
---

With `yes`, a [`Splits=yes`](/keys/splits/) projectile builds its list of candidate targets around its target's position, and an [`AirburstSpread`](/keys/airburstspread/) projectile targets the cells around the cell its target is in. With `no`, both work around the cell the projectile burst over. A splitting projectile whose target is a cell rather than an object has no position to look around, so it uses its own.

```ini title="rulesmd.ini"
[MyBurst] ; example BulletType
Airburst=yes
AirburstWeapon=MyBomblet
AirburstSpread=2
AroundTarget=yes
```
