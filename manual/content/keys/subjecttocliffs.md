---
key: SubjectToCliffs
summary: "Makes a projectile explode against a cliff in its path."
see_also: [SubjectToWalls]
when_omitted:
  kind: value
  value: "no"
---

A projectile with `SubjectToCliffs=yes` explodes as soon as it enters a cell at least four height levels above the cell it left, if that cell is also higher than the cell it was fired from. A shot fired from the cliff top therefore flies on.

```ini title="rulesmd.ini"
[Cannon] ; projectile
SubjectToCliffs=yes
```
