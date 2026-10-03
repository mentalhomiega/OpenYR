---
key: JumpjetAccel
summary: "How fast a jumpjet of this type speeds up."
see_also: [Acceleration]
when_omitted:
  kind: value
  value: "[JumpjetControls] Acceleration"
---

Sets how many leptons a frame a jumpjet of this type gains each frame until it reaches its speed. It slows down one and a half times as fast. A type without the key uses `Acceleration` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetAccel=2
```
