---
key: ChronoHarvTooFarDistance
summary: Cells a Chrono Miner may be from a free refinery and still reserve it instead of driving to the nearest one.
see_also: ["system:tiberium", "HarvesterTooFarDistance", "Teleporter"]
when_omitted:
  kind: value
  value: "50"
---

```ini title="rules.ini"
[General]
ChronoHarvTooFarDistance=50
```

A Chrono Miner returning with a load reserves the nearest free refinery of its [`Dock`](/keys/dock/) list only when that refinery is within this many cells, measured in a straight line. Otherwise it drives to the nearest refinery of any kind and waits there. [`HarvesterTooFarDistance`](/keys/harvestertoofardistance/) sets the limit for every other harvester.
