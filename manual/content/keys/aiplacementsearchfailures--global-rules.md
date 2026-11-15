---
key: AIPlacementSearchFailures
scope: global-rules
label: Failed placement searches before a computer node is dropped
summary: The number of times the placement search may find no place for a computer house's structure before its base node is removed from the plan.
see_also: [MaximumBuildingPlacementFailures, AIBaseSpacing, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "3"
---

A whole number; this key is ours. When a computer house finishes a structure and the [placement search](/systems/ai-base-building/#the-placement-search) finds no cell for it, the structure is abandoned and its cost refunded, and its base node counts one failed search. In a skirmish or multiplayer game, a node whose count reaches this value is removed from the plan, and the house builds the next structure in its plan. `0` keeps the node for as long as the game lasts, which is what Yuri's Revenge does: the house then starts the same structure again after each failure and builds nothing further down its plan. A campaign game never removes the node.

An upgrade that has no structure to go on is removed after its first failed search when it is the node the house would build next, whatever this value is.

```ini title="rulesmd.ini"
[AI]
AIPlacementSearchFailures=5
```
