---
key: NormalTargetingDelay
summary: "Frames between an OpportunityFire object's target scans on the move."
see_also: [OpportunityFire, GuardAreaTargetingDelay]
when_omitted:
  kind: value
  value: "27"
---

An [`OpportunityFire=yes`](/keys/opportunityfire/) object that is moving or harvesting [scans for a target in range](/systems/target-selection/#mission-entry-points) once every this many frames.

```ini title="rulesmd.ini"
[General]
NormalTargetingDelay=27
```
