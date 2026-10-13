---
key: NormalTargetingDelay
summary: "Frames between an OpportunityFire object's target scans on the move."
see_also: [OpportunityFire, Aggressive, GuardAreaTargetingDelay]
when_omitted:
  kind: value
  value: "27"
---

An [`OpportunityFire=yes`](/keys/opportunityfire/) object that is moving or harvesting [scans for a target in range](/systems/target-selection/#mission-entry-points) once every this many frames. A computer's member of an [aggressive](/keys/aggressive/) team scans on the same interval while it moves.

```ini title="rulesmd.ini"
[General]
NormalTargetingDelay=27
```
