---
key: AIExtraCounts
summary: The most extra copies of the BuildingType a computer house may add to its plan at random, per difficulty slot.
see_also: [AIBuildCounts, AIBuildThis, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "`0,0,0`, no extra copies"
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. When a computer house generates its base plan, it adds a random number of copies of the type from `0` up to the number for its slot, on top of the copies [`AIBuildCounts`](/keys/aibuildcounts/) plans. Each extra copy is inserted after a random entry from the type's first copy on, in the same way as the planned copies.

A value below `0` and a slot past the end of the list both count as `0`.

```ini title="rulesmd.ini"
[GAPILLBOX] ; example BuildingType
AIBuildThis=yes
AIBuildCounts=2,2,1
AIExtraCounts=2,1,0 ; two to four copies on the first slot
```
