---
key: JumpjetSpeed
summary: "The top speed of a jumpjet of this type."
see_also: [Speed]
when_omitted:
  kind: value
  value: "[JumpjetControls] Speed"
---

Sets the top speed of a jumpjet of this type in the air, in leptons a frame. It slows to half this within two cells of where it is going and to three tenths within one. A type without the key uses `Speed` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetSpeed=30
```
