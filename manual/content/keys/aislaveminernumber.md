---
key: AISlaveMinerNumber
summary: How many ore gatherers a computer house keeps and plans for, per difficulty slot, when it can build no harvester or no refinery.
see_also: [AIExtraRefineries, AIRestrictReplaceTime, HarvestersPerRefinery, HarvesterUnit, "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. A slot past the end of the list counts as `0`. A computer house reads its slot's entry in four places:

- When its side has a base defense count list and its country may own none of the [`HarvesterUnit`](/keys/harvesterunit/) entries, it plans one fewer extra copy of its first refinery than the entry. The [generated base plan](/systems/ai-base-building/#the-counted-plan) places each copy after a random entry from that refinery on.
- When no [`BuildRefinery`](/keys/buildrefinery/) entry is buildable for it, it keeps this many ore gatherers in place of [`HarvestersPerRefinery`](/keys/harvestersperrefinery/) for each refinery.
- When its country may own no `HarvesterUnit` entry, it orders the slave miner its first refinery packs up into until it has this many ore gatherers.
- It rebuilds a lost structure with both `ResourceGatherer=yes` and `ResourceDestination=yes` only while it has fewer ore gatherers than this, as [Rebuilding](/systems/ai-base-building/#rebuilding) describes.

```ini title="rulesmd.ini"
[General]
AISlaveMinerNumber=3,2,1 ; slot 0 plans two extra refineries and keeps three gatherers; slot 2 plans none and keeps one
```
