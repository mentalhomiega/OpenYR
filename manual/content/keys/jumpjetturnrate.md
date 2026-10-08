---
key: JumpjetTurnRate
summary: "How fast a jumpjet of this type turns."
see_also: [TurnRate]
when_omitted:
  kind: value
  value: "[JumpjetControls] TurnRate"
---

Sets how fast a jumpjet of this type turns while it flies, in the units [`TurnRate`](/keys/turnrate/) uses. A type without the key uses `TurnRate` from `[JumpjetControls]`.

The turn also slows the approach to the destination. While the heading still to be turned is more than this many 256ths of a circle, the jumpjet's speed drops to a tenth of its top speed in the step that applies within two top speeds of the destination. While the heading is more than five times this many 256ths away, the speed drops to a fifth of its top speed in the step within 50 times the top speed divided by this value. Each of these reduced speeds is at least one lepton a frame. See [`JumpjetSpeed`](/keys/jumpjetspeed/).

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetTurnRate=4
```
