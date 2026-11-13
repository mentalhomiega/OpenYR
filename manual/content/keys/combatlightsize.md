---
key: CombatLightSize
summary: "The size of the light flash this warhead's explosion makes, as a share of the largest flash."
see_also: [Bright, "system:destruction-and-debris"]
when_omitted:
  kind: value
  value: "0, so the damage sets the size"
---

`CombatLightSize` is an Ares key. An explosion of a [`Bright`](/keys/bright/) warhead lights up the ground around it. With the key above `0`, the flash has that share of the largest size: `0.5` gives half of it, cut to a whole number, and anything above `1.0` counts as `1.0`. With `0` or below, the damage sets the size as before: the damage times 64, divided by 256, never below 21 or above 63. The largest flash is 63.

The key sets only the size. A warhead without `Bright=yes` still makes no flash, except where the game forces one.

```ini title="rulesmd.ini"
[IonBlast]
Bright=yes
CombatLightSize=0.8
```
