---
key: OmniCrusher.Aggressive
scope: aircrafttype
summary: "With no, an OmniCrusher vehicle does not drive at its target to flatten it."
see_also: [OmniCrusher, OmniCrushResistant, Crusher]
when_omitted:
  kind: value
  value: "yes"
---

An [`OmniCrusher=yes`](/keys/omnicrusher/) vehicle that has a target it can flatten drives over it, for a human player as well. With `OmniCrusher.Aggressive=no` it approaches as any other vehicle does and fires from its weapon range. What it flattens on its way, and what a move order across a crushable object does, are not changed.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
OmniCrusher=yes
OmniCrusher.Aggressive=no
```
