---
key: CanRecalcApproachTarget
scope: aircrafttype
label: 'Picks a new firing spot when the target moves'
see_also: [CanApproachTarget, ApproachTargetResetMultiplier]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
CanRecalcApproachTarget=no
```

An object heading for a spot to shoot from drops that spot when its target moves too far from it, and picks a new one. The object drops the spot while its target is out of weapon reach and farther from the spot than [`ApproachTargetResetMultiplier`](/keys/approachtargetresetmultiplier/) times its weapon's range, less half a cell for a range of two cells or more.

With `no`, the object keeps the spot it chose and drives there even after the target has moved away.
