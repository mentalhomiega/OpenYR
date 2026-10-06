---
key: AttachEffect.SpeedMultiplier
scope: warheadtype
label: Speed multiplier
when_omitted:
  kind: value
  value: "1.0"
  note: "Speed is unchanged."
---

`AttachEffect.SpeedMultiplier` multiplies the movement speed of each object carrying this warhead's effect: `0.5` halves it and `2` doubles it. It applies to infantry, vehicles and ships; an object that moves with the aircraft or jumpjet locomotor keeps its speed. With several effects attached, their multipliers are [multiplied together](/systems/attach-effects/#multipliers).

```ini title="rulesmd.ini"
[SlowGoo] ; example Warhead
AttachEffect.Duration=300
AttachEffect.SpeedMultiplier=0.5 ; an object moving at 40 slows to 20
```
