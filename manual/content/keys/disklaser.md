---
key: DiskLaser
summary: "Makes a laser weapon draw a ring around its firer before its beam strikes."
see_also: [DiskLaserChargeUp, LaserInnerColor, LaserDuration]
when_omitted:
  kind: value
  value: "no"
---

Fired at an object, a weapon with `DiskLaser=yes` fires no projectile. Lasers run around a ring 240 leptons out from the firer's firing point, starting on the side away from the target and drawing one segment along each side every other frame. When the two arcs meet, about 16 frames later, a beam from the ring strikes the target and deals the weapon's `Damage` with its `Warhead` there, and the weapon's `Report` plays.

The ring and beam use the weapon's `LaserInnerColor`, `LaserOuterColor`, `LaserOuterSpread` and `LaserDuration`, or the owner's color with `IsHouseColor=yes`. The shot is abandoned, with no damage, if the target leaves the weapon's `Range` or the firer is destroyed before the ring closes. The range runs from the firer's position to the target's position, less `(Width + Height) * 64` leptons of a building's foundation. While the firer is in the air, its height does not count. A new shot waits until the current ring has closed. At a target that is not an object, such as the ground, the weapon fires its `Projectile` as usual.

```ini title="rulesmd.ini"
[MyDiscLaser] ; example Weapon
DiskLaser=yes
Damage=90
ROF=80
Range=7
Warhead=MyDiscWH
LaserInnerColor=216,0,184
LaserOuterColor=80,0,88
LaserDuration=15
```
