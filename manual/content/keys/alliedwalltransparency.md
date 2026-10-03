---
key: AlliedWallTransparency
summary: "Lets shots fly through walls owned by the firer's allies."
see_also: [SubjectToWalls]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a [`SubjectToWalls=yes`](/keys/subjecttowalls/) projectile flies through a wall owned by its firer's house or an ally instead of exploding on it.

```ini title="rulesmd.ini"
[WallModel]
AlliedWallTransparency=no
```
