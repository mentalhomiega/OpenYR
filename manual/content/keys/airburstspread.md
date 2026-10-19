---
key: AirburstSpread
summary: "Makes an Airburst projectile fire its AirburstWeapon at every cell within this many cells of the burst."
see_also: [Airburst, AirburstWeapon, AroundTarget, Splits, "system:projectile-flight"]
when_omitted:
  kind: value
  value: "not set, so an Airburst projectile splits as `Splits` describes"
---

An [`Airburst=yes`](/keys/airburst/) projectile that sets `AirburstSpread` does not split. It detonates once and then fires its [`AirburstWeapon`](/keys/airburstweapon/) once at each cell whose centre is within this many cells of the cell it burst over, or of its target with [`AroundTarget=yes`](/keys/aroundtarget/). A value under `1` targets that one cell, `1` adds its four neighbours, and `1.5` fills the three-by-three block around it. [`Cluster`](/keys/cluster/) is not used, and the projectile detonates once whatever it is.

Each bomblet takes the weapon's projectile, warhead and [`Damage`](/keys/damage--weapontype/) as they are, with no multiplication by ten, and starts from the burst point at the weapon's [`Speed`](/keys/speed/#scope-weapontype), pointing straight down. A weapon with no projectile fires nothing. Write `Splits=yes` in the same section to keep the splitting instead of the spread.

```ini title="rulesmd.ini"
[MyBurst] ; example BulletType
Airburst=yes
AirburstWeapon=MyBomblet
AirburstSpread=1.5
```
