---
key: CanPassiveAquire
summary: "With no, an idle object of this type never picks a target on its own."
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: "yes"
---

A `CanPassiveAquire=no` object skips the [Guard and Guard area scans and the opportunity scans on a move order](/systems/target-selection/#mission-entry-points), so it attacks only what it is ordered to, or what a hunting or team mission finds. A computer's member of an [aggressive](/keys/aggressive/) team still scans on a move order. The spelling of the key is as the game reads it.

```ini title="rulesmd.ini"
[MYDEMOTRUCK] ; example VehicleType
CanPassiveAquire=no
```
