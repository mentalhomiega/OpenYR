---
key: OpportunityFire
summary: "Lets a moving or harvesting object of this type shoot at targets in range without stopping."
see_also: [NormalTargetingDelay, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

An armed object of an `OpportunityFire=yes` type [looks for targets in range](/systems/target-selection/#mission-entry-points) while it moves or harvests, and fires at them without leaving its route. A type with [`CanPassiveAquire=no`](/keys/canpassiveaquire/) does not.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
OpportunityFire=yes
```
