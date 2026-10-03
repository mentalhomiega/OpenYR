---
key: SubjectToWalls
summary: "Makes a projectile explode on a wall in its path."
see_also: [SubjectToCliffs, AlliedWallTransparency, "system:walls-and-gates"]
when_omitted:
  kind: value
  value: "no"
---

A projectile with `SubjectToWalls=yes` explodes as soon as it enters a cell holding a wall, unless that cell is its target's. A shot fired from higher ground than its target's cell flies over walls. With [`AlliedWallTransparency=yes`](/keys/alliedwalltransparency/), a shot also flies through walls owned by its firer's allies.

```ini title="rulesmd.ini"
[Cannon] ; projectile
SubjectToWalls=yes
```
