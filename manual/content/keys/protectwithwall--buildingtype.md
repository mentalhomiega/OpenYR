---
key: ProtectWithWall
scope: buildingtype
label: Walled by the computer
see_also: [AIPickWallDefensePercent, ConcreteWalls, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "no"
---

The flag makes a computer house ring the structure with walls. When a defense turn in its [base plan](/systems/ai-base-building/#protective-walls) takes the wall roll, the house walls the last flagged structure before that turn whose next plan node is not already a wall. The ring runs one cell outside the structure's foundation on every side, and the walls are built ahead of the defense. A structure whose next plan node is a wall is skipped, so a ring is not added twice.

```ini title="rulesmd.ini"
[GACNST]
ProtectWithWall=yes
```

The shipped rules flag the construction yards, tech centers, superweapon structures and similar high-value types. A flagged structure is never walled when the country may own no [`ConcreteWalls`](/keys/concretewalls/) entry.
