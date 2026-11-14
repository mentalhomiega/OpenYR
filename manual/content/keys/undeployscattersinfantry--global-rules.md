---
key: UndeployScattersInfantry
scope: global-rules
label: Soldiers on an undeploy cell
summary: Whether allied soldiers standing on the cell a structure undeploys onto are moved aside and waited for.
see_also: ["UndeploysInto"]
when_omitted:
  kind: value
  value: "no"
  note: gamemd gives up on the vehicle when a soldier stands in its way, so `no` matches the original engine.
---

```ini title="rules.ini"
[General]
UndeployScattersInfantry=no
```

With `no`, a structure sold into its vehicle needs the cell the vehicle appears on to be free of soldiers at that moment. A soldier standing or walking there stops the vehicle from appearing. The structure is then removed and its owner is refunded the sale price. A deployed Slave Miner that is sold while one of its slaves is on that cell therefore frees all of its slaves. The vehicle's cell lies outside the structure only when the structure is a single row or column of more than 2 cells ([`UndeploysInto`](/keys/undeploysinto/) gives the cell), so the stock 2x2 Slave Miner refinery is never blocked this way.

With `yes`, allied soldiers on that cell are ordered to step aside, and the structure waits for up to five seconds for the cell to clear. The vehicle then appears as usual and keeps its slaves. A vehicle or an enemy on the cell does not delay the undeploy, so it fails as with `no`. The same refund applies if the cell is still occupied after five seconds.
