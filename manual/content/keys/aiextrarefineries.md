---
key: AIExtraRefineries
summary: How many extra refineries a computer house plans, per difficulty slot, when it can build a harvester.
see_also: [AISlaveMinerNumber, AlliedBaseDefenseCounts, BuildRefinery, HarvesterUnit, "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. It applies to a computer house whose side has a base defense count list, such as [`AlliedBaseDefenseCounts`](/keys/alliedbasedefensecounts/). When that house's country may own any [`HarvesterUnit`](/keys/harvesterunit/) entry, its [generated base plan](/systems/ai-base-building/#the-counted-plan) gets the number for its slot of extra copies of its first refinery, each after a random entry from that refinery on.

```ini title="rulesmd.ini"
[General]
AIExtraRefineries=2,1,0
```

A house whose country can build no harvester takes its count from [`AISlaveMinerNumber`](/keys/aislaveminernumber/) instead. A slot past the end of the list counts as `0`.
