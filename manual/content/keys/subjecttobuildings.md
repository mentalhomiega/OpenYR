---
key: SubjectToBuildings
summary: "Makes a projectile explode on a building that is solid to it, so that it cannot fly through."
see_also: [SolidHeight, AlliedSolidTransparency, SubjectToWalls, "system:projectile-flight"]
when_omitted:
  kind: value
  value: "no"
---

A projectile with `SubjectToBuildings=yes` explodes where it stands when it enters a cell holding a building whose [`SolidHeight`](/keys/solidheight/) is not `0` and is higher than the projectile is above the ground. A shot that flies above the building's solid height is not stopped. The building that fired the shot never stops it, an open gate never does, and with [`AlliedSolidTransparency=yes`](/keys/alliedsolidtransparency/) neither does a building owned by an ally of the shooter.

A unit does not look for a clearer line of fire before shooting. The shot explodes on the building and the building takes the damage.

```ini title="rulesmd.ini"
[MyShell] ; example BulletType
SubjectToBuildings=yes
```
