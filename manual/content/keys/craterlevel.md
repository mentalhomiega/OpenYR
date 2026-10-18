---
key: CraterLevel
summary: How many cells around a meteor impact are slumped into a crater.
see_also: [IsMeteor]
when_omitted:
  kind: value
  value: "0"
---

The value selects how far a meteor's crater spreads from the impact cell. The default, `0`, leaves the terrain unchanged when a meteor lands. The original game never reads this key, so a meteor makes a crater only when a mod or map sets a value above `0`. The stock rules file's `CraterLevel=1` is ignored.

The value does not change how deep each cell slumps:

| Value | Cells deformed |
| --- | --- |
| `0` | none |
| `1` | the impact cell |
| `2` | the impact cell and its four corner neighbors (north-east, south-east, south-west and north-west) |
| `3` | the impact cell and all eight neighbors |
| `4` or more | as `3`, and the impact cell a second time |

Only an animation or voxel animation with [`IsMeteor=yes`](/keys/ismeteor/) uses this setting, when it lands. A meteor that lands on water or on a bridge deck deforms nothing. A warhead that reshapes the ground uses its own [`Deform`](/keys/deform/) and [`DeformThreshhold`](/keys/deformthreshhold/) and affects only the cell it hits.

A cell deforms only when all eight of its neighbors lie inside the playfield. A crater near the edge of the playfield therefore comes out clipped.

:::caution[Only the last cell of a crater slumps completely]
Each cell slumps in two steps. One random corner drops at once, and the other three drop five frames later. The game holds only one pending second step at a time, so each further cell that deforms replaces it. At `2` or more, every cell except the last one deformed ends up with only one corner lowered. At `4`, the last cell is the impact cell.
:::
