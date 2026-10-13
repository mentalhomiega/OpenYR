---
key: QueueingCell
summary: The cell, counted from a structure's top-left cell, where a harvester waits for the structure to become free.
see_also: ["system:tiberium", "Dock", "HarvesterTooFarDistance", "ChronoHarvTooFarDistance"]
when_omitted:
  kind: value
  value: "0,0"
---

Two whole numbers, `X,Y`, in cells. They are added to the structure's top-left foundation cell to give the cell a harvester or weeder drives to when it cannot reserve this structure. The unit stops on the free cell nearest that spot.

The offset is used when the structure is the nearest [`Dock`](/keys/dock/) structure of any kind, no free one is within reach, and the structure is more than 3 cells from the unit. A Chrono Miner uses it at any distance. [Unloading](/systems/tiberium/#unloading) covers when a harvester reserves a structure and when it waits instead.

```ini title="art.ini"
[GAREFN] ; the Image ID of a BuildingType with Foundation=4x3
QueueingCell=4,1 ; one cell past the structure's 4-cell width, in its second row
```

With the default `0,0` the target is the structure's own top-left cell, so the unit stops on the free cell nearest the corner of the structure. Both stock refineries set `4,1`. The search checks the cells for a wheeled vehicle in the normal movement zone. It ignores the cell's zone and height level, and it accepts a cell under a bridge.
