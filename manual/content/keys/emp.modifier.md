---
key: EMP.Modifier
summary: Scales how long an EM pulse stuns this object.
see_also: [EMP.Threshold, ImmuneToEMP, "system:emp-pulse"]
when_omitted:
  kind: value
  value: "1.0, so the stun lasts the pulse's full duration"
---

An EM pulse that stuns this object sets its stun to the pulse's duration multiplied by this value, rounded down to whole frames. Write the value as a fraction, as in `0.5`, or as a percentage, as in `50%`. A value above `1` lengthens the stun and a value below `1` shortens it.

The setting applies wherever a pulse stuns an object: structures, vehicles, aircraft standing on the ground, cyborgs, and objects traveling underground. An aircraft in the air is crashed or destroyed by the pulse and is not stunned, so the value does not affect it. A type that is [immune](/keys/immunetoemp/) is not stunned at all.

A stun that the value makes long enough can also destroy the object, as [`EMP.Threshold`](/keys/emp.threshold/) describes.

```ini title="rulesmd.ini"
[HTNK] ; example VehicleType that recovers from a pulse in half the time
EMP.Modifier=50%
```
