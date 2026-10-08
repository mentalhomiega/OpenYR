---
key: JumpjetSpeed
summary: "The top speed of a jumpjet of this type."
see_also: [Speed]
when_omitted:
  kind: value
  value: "[JumpjetControls] Speed"
---

Sets the top speed of a jumpjet of this type in the air, in leptons a frame. As it closes on its destination, the jumpjet steps down from this speed: to an eighth of it within one top speed of the destination, to a quarter within two top speeds, and to half within this speed times 50, divided by [`JumpjetTurnRate`](/keys/jumpjetturnrate/). Farther out, it flies at full speed. Distances are in leptons, 256 to a cell. A turn well away from the heading slows it further, as [`JumpjetTurnRate`](/keys/jumpjetturnrate/) describes. A type without the key uses `Speed` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetSpeed=30
```
