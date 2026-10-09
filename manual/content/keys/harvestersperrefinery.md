---
key: HarvestersPerRefinery
summary: How many ore gatherers a computer house keeps for each refinery it owns, per difficulty slot.
see_also: [AISlaveMinerNumber, HarvesterUnit, ResourceGatherer, "system:tiberium"]
when_omitted:
  kind: value
  value: none
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. A computer house that may own a [`HarvesterUnit`](/keys/harvesterunit/) orders a replacement harvester while it has fewer ore gatherers than this number times the refineries it owns. Gatherers are the types with [`ResourceGatherer=yes`](/keys/resourcegatherer/), slave miners included.

```ini title="rulesmd.ini"
[General]
HarvestersPerRefinery=2,2,1
```

A house with no refinery type buildable keeps [`AISlaveMinerNumber`](/keys/aislaveminernumber/) gatherers instead. A slot past the end of the list counts as `0`, so the computer orders no harvester at that slot.
