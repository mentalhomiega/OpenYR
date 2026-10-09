---
key: MaximumBuildingPlacementFailures
scope: global-rules
label: Blocked placements before a computer node is dropped
summary: The number of times a computer house's structure may be blocked on its foundation before its base node is removed from the plan.
see_also: [PlacementDelay, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "5"
---

A whole number. When a computer house finishes a structure but a unit is still standing on its foundation, the attempt is blocked and the construction yard waits [`PlacementDelay`](/keys/placementdelay/) minutes before trying again. Each blocked attempt adds one to that node's count. In a skirmish or multiplayer game, a node whose count passes this value is removed from the plan, and its structure is placed at a cell the placement search finds. A campaign game never removes the node.

```ini title="rulesmd.ini"
[General]
MaximumBuildingPlacementFailures=3
```
