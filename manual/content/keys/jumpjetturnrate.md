---
key: JumpjetTurnRate
summary: "How fast a jumpjet of this type turns."
see_also: [TurnRate]
when_omitted:
  kind: value
  value: "[JumpjetControls] TurnRate"
---

Sets how fast a jumpjet of this type turns while it flies, in the units [`TurnRate`](/keys/turnrate/) uses. A type without the key uses `TurnRate` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetTurnRate=4
```
