---
key: JumpjetClimb
summary: "How fast a jumpjet of this type climbs and descends."
see_also: [Climb]
when_omitted:
  kind: value
  value: "[JumpjetControls] Climb"
---

Sets the leptons of altitude a jumpjet of this type gains or loses each frame, as [`Climb`](/keys/climb/) describes. A type without the key uses `Climb` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetClimb=20
```
