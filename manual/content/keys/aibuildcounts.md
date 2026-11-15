---
key: AIBuildCounts
summary: How many copies of the BuildingType a computer house plans, per difficulty slot.
see_also: [AIExtraCounts, AIBuildThis, AIExtraRefineries, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "`1,1,1`, one copy"
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. When a computer house [generates its base plan](/systems/ai-base-building/#building-the-plan), the BuildingType enters it as many times as the house's slot says. The first copy keeps the place the plan gives the type. Each further copy is inserted after a random entry from that first copy on, so it never comes before the type's prerequisites.

A value below `1` and a slot past the end of the list both count as `1`. The key only changes types that the plan already includes, such as those with [`AIBuildThis=yes`](/keys/aibuildthis/). It has no effect on a house that follows a node list supplied by the scenario.

The key also applies to a type that has its own planning rules, such as the first [`BuildRefinery`](/keys/buildrefinery/) entry, whose copies add to those from [`AIExtraRefineries`](/keys/aiextrarefineries/).

```ini title="rulesmd.ini"
[GAPILLBOX] ; example BuildingType
AIBuildThis=yes
AIBuildCounts=4,3,2 ; four on the first slot, two on the third
```
