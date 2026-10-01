---
key: IsHouseColor
summary: "Draws the weapon's laser in its firer's house color."
see_also: [IsLaser, LaserInnerColor, LaserOuterColor, LaserOuterSpread]
when_omitted:
  kind: value
  value: "no"
---

An [`IsLaser=yes`](/keys/islaser/) weapon with `IsHouseColor=yes` draws its beam with the firer's house color as the core and half that color around it, with no outer spread. [`LaserInnerColor`](/keys/laserinnercolor/), [`LaserOuterColor`](/keys/laseroutercolor/) and [`LaserOuterSpread`](/keys/laserouterspread/) are then ignored.

```ini title="rulesmd.ini"
[MyPrismShot] ; example Weapon
IsLaser=yes
IsHouseColor=yes
```
