---
key: HarvesterTooFarDistance
summary: Cells a harvester may be from a free refinery and still reserve it instead of waiting at the nearest one.
see_also: ["system:tiberium", "ChronoHarvTooFarDistance", "Dock"]
when_omitted:
  kind: value
  value: "5"
---

```ini title="rules.ini"
[General]
HarvesterTooFarDistance=5
```

A harvester returning with a load reserves the nearest free refinery of its [`Dock`](/keys/dock/) list only when that refinery is within this many cells, measured in a straight line. A farther free refinery is not reserved. The harvester drives to the nearest refinery of any kind and waits there until a free one comes within this distance. [Unloading](/systems/tiberium/#unloading) describes the wait. A Chrono Miner uses [`ChronoHarvTooFarDistance`](/keys/chronoharvtoofardistance/) instead.
