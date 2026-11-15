---
key: AIBaseNormal
summary: Whether the structure extends the base space in which its owner's computer-controlled buildings are placed.
see_also: [BaseNormal, AIBaseSpacing, "system:ai-base-building"]
when_omitted:
  kind: context-dependent
  note: "`yes` unless the structure is the deployed form of a vehicle other than a construction yard, or a deployed miner, which are `no`."
---

A structure that extends the base space adds the cells around it to the area a computer house searches when it places a new structure. Without this key every structure does so, except a deployed vehicle that is not a construction yard. [`BaseNormal`](/keys/basenormal/) does not change this, because it only decides which structures a human player may build next to.

`AIBaseNormal=no` keeps the structure from widening the base, which suits a structure that is placed away from the base, such as a naval yard or a civilian building that the house captured. `AIBaseNormal=yes` makes a structure widen it even when the default would not.

```ini title="rulesmd.ini"
[NAYARD] ; example BuildingType
AIBaseNormal=no
```
