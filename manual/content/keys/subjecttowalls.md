---
key: SubjectToWalls
summary: "Makes a projectile explode on a wall in its path."
see_also: [SubjectToCliffs, AlliedWallTransparency, "system:walls-and-gates"]
when_omitted:
  kind: value
  value: "no"
---

A projectile with `SubjectToWalls=yes` explodes as soon as it enters a cell holding a wall, unless that cell is its target's. A shot fired from higher ground than its target's cell flies over walls. With [`AlliedWallTransparency=yes`](/keys/alliedwalltransparency/), a shot also flies through walls owned by its firer's allies.

A unit does not fire at a target when a wall on the line to it would stop the projectile. A unit on an attack order looks for a nearby cell with a clear line and moves there; if it finds none, it moves toward the target. The exception is a warhead with [`Wall=yes`](/keys/wall/), which may fire past a wall at overlay position 2, 26 or 243 of `[OverlayTypes]` (`GAWALL`, `NAWALL` and `GAFWLL` in the stock list). That projectile still stops at the first wall it reaches, as above.

```ini title="rulesmd.ini"
[Cannon] ; projectile
SubjectToWalls=yes
```
