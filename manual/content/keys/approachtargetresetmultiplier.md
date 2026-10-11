---
key: ApproachTargetResetMultiplier
summary: "How far a moving target may get from the spot an object is heading to, in weapon ranges, before the object picks a new spot."
see_also: [CanRecalcApproachTarget, "system:slave-miners"]
when_omitted:
  kind: value
  value: "1"
---

```ini title="rulesmd.ini"
[General]
ApproachTargetResetMultiplier=1
```

The value is a whole number, so `1.5` reads as `1`.

An object heading for a spot to shoot from drops that spot, and picks another, once its target is out of weapon reach and farther from the spot than this many of its weapon's ranges. The range counts less half a cell when it is two cells or more. [`CanRecalcApproachTarget=no`](/keys/canrecalcapproachtarget/#scope-aircrafttype) turns this off for a type.

A [slave miner's](/systems/slave-miners/) slave walking back to its miner is sent again when the cell it is heading for is more than this many cells from the cell it would be sent to now, as when the miner has moved. The slave is sent again on the next check, which comes every ten frames.
