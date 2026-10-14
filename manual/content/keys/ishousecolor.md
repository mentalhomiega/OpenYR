---
key: IsHouseColor
summary: "Draws the weapon's laser in its firer's house laser color."
see_also: [IsLaser, LaserInnerColor, LaserOuterColor, LaserOuterSpread]
when_omitted:
  kind: value
  value: "no"
---

An [`IsLaser=yes`](/keys/islaser/) weapon with `IsHouseColor=yes` draws its beam in the firer's house laser color as the core, and half that color around it. [`LaserInnerColor`](/keys/laserinnercolor/) and [`LaserOuterColor`](/keys/laseroutercolor/) are ignored. The glow's spread still comes from [`LaserOuterSpread`](/keys/laserouterspread/), as it does for any laser.

The house laser color is the house color scaled to a brightness magnitude of 240, with each channel capped at 255. Channels below 96 are then set to zero, and the result is scaled to magnitude 240 again. A black house color therefore gives 138 on each channel.

```ini title="rulesmd.ini"
[MyPrismShot] ; example Weapon
IsLaser=yes
IsHouseColor=yes
```
