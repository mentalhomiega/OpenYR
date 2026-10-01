---
key: CanPassiveAquire
summary: "With no, an idle object of this type never picks a target on its own."
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: "yes"
---

A `CanPassiveAquire=no` object skips the [Guard, Guard area and Move scans](/systems/target-selection/#mission-entry-points), so it attacks only what it is ordered to, or what a hunting or team mission finds. The spelling of the key is as the game reads it.

```ini title="rulesmd.ini"
[MYDEMOTRUCK] ; example VehicleType
CanPassiveAquire=no
```
