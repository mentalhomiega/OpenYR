---
key: IvanIconFlickerRate
summary: "How many frames the Ivan bomb icon spends in each half of its flicker."
see_also: [IvanTimedDelay, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "0"
---

The countdown icon over an object carrying the player's [Ivan bomb](/systems/ivan-bombs/#the-countdown) switches between its two frames every this many game frames. With `0`, it does not flicker.

```ini title="rulesmd.ini"
[CombatDamage]
IvanIconFlickerRate=8
```
