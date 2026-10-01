---
key: AlliedBaseDefenseCounts
summary: How many base defenses a computer house of the first side plans, per difficulty slot.
see_also: [SovietBaseDefenseCounts, ThirdBaseDefenseCounts, AlliedBaseDefenses, AIExtraRefineries, "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
  note: Without a list, a house of the first side plans its defenses from the cost of its plan.
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. A computer opponent on Hard normally holds slot 0, on Normal slot 1 and on Easy slot 2. A computer house whose country is on the first side in `[Sides]` puts the number for its slot of base-defense placeholders into its [generated base plan](/systems/ai-base-building/#the-counted-plan), each after a random entry from the fourth on. The defense planner later fills each placeholder from [`AlliedBaseDefenses`](/keys/alliedbasedefenses--global-rules/).

```ini title="rulesmd.ini"
[General]
AlliedBaseDefenseCounts=20,15,8
```

Setting the list also gives the side the rest of the counted plan: no extra helipads, extra refineries from [`AIExtraRefineries`](/keys/aiextrarefineries/), and no perimeter wall. A slot past the end of the list counts as `0`.
