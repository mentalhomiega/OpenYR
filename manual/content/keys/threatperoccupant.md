---
key: ThreatPerOccupant
summary: "How much each soldier inside a garrisoned structure adds to the structure's threat."
see_also: ["system:garrisons", ThreatPosed, "system:base-attacked"]
when_omitted:
  kind: value
  value: "5"
---

```ini title="rulesmd.ini"
[General]
ThreatPerOccupant=10
```

A structure with soldiers inside adds this value for each of them to [the threat map](/systems/base-attacked/#the-threat-map), and its type's [`ThreatPosed`](/keys/threatposed/) counts for nothing while anyone is inside. An empty structure goes back to its `ThreatPosed`. The map follows soldiers moving in, and the soldiers being ejected, killed or removed.
