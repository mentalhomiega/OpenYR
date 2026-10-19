---
key: AirburstSpread
summary: "Sets which cells an Airburst projectile fires its AirburstWeapon at: every cell within this many cells of the burst."
see_also: [Airburst, AirburstWeapon, AroundTarget, Splits, "system:projectile-flight"]
when_omitted:
  kind: value
  value: "`1.5` for the stock launch, which fills the three-by-three block"
---

An [`Airburst=yes`](/keys/airburst/) projectile detonates once and then fires its [`AirburstWeapon`](/keys/airburstweapon/) once at each cell whose centre is within this many cells of the cell it burst over, or of its target with [`AroundTarget=yes`](/keys/aroundtarget/). A value under `1` targets that one cell, `1` adds its four neighbours, and `1.5` fills the three-by-three block around it. [`Cluster`](/keys/cluster/) is not used. A projectile that writes `Splits=yes` splits instead and ignores this key.

Without this key, the projectile fires at the cell it burst over and its eight neighbours, neighbours first and the centre last. Each bomblet takes the weapon's projectile, warhead and [`Damage`](/keys/damage--weapontype/) as they are, with no multiplication by ten, and starts from the burst point at the weapon's [`Speed`](/keys/speed/#scope-weapontype), pointing straight down. A weapon with no projectile fires nothing.

Without this key a homing bomblet's top speed is 50 leptons per game frame and it throws no lighting flash, and `AroundTarget` has no effect. Writing the key, even as `1.5`, sets the top speed to the weapon's `Speed`, lets the weapon's [`Bright`](/keys/bright--weapontype/) setting decide the flash, and honours `AroundTarget`.

```ini title="rulesmd.ini"
[MyBurst] ; example BulletType
Airburst=yes
AirburstWeapon=MyBomblet
AirburstSpread=1.5
```
