---
key: AISlaveMinerNumber
summary: One more than the extra refineries a computer house plans, per difficulty slot, when it can build no harvester.
see_also: [AIExtraRefineries, HarvesterUnit, "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. A computer house whose side has a base defense count list, and whose country may own none of the [`HarvesterUnit`](/keys/harvesterunit/) entries, plans one fewer extra copy of its first refinery than the number for its slot. The [generated base plan](/systems/ai-base-building/#the-counted-plan) places each copy after a random entry from that refinery on.

```ini title="rulesmd.ini"
[General]
AISlaveMinerNumber=3,2,1 ; two, one and no extra refineries
```

A slot past the end of the list counts as `0`, which plans no extra refinery. The value has no other effect.
