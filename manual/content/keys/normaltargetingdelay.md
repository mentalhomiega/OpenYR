---
key: NormalTargetingDelay
summary: "Frames between an OpportunityFire object's target scans on the move."
see_also: [OpportunityFire, Aggressive, GuardAreaTargetingDelay]
when_omitted:
  kind: value
  value: "27"
---

An object on a move, harvest or guard mission that can [opportunity-fire](/keys/opportunityfire/) [scans for a target in range](/systems/target-selection/#mission-entry-points) each time its [targeting timer](/systems/target-selection/#the-targeting-timer) runs out. Each such scan restarts the timer with this many frames plus a random 0 to 2. A hunt scan restarts it the same way. A computer's member of an [aggressive](/keys/aggressive/) team scans on a move order in the same way.

```ini title="rulesmd.ini"
[General]
NormalTargetingDelay=27
```
