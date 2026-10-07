---
key: PoweredSpecial
summary: "Takes a power plant out of service during a spy's blackout and while it is drained, and swaps its animations."
see_also: ["system:power"]
when_omitted:
  kind: value
  value: "no"
---

`PoweredSpecial=yes` makes a structure stop working while its house is in a power blackout from a spy, or while one of its power plants is drained. While it is out of service, the animation slots whose art section sets `PoweredSpecial=yes` (for example `ActiveAnimPoweredSpecial=yes`) stop, and its `LowPower` animation plays in their place. When it works again they start and the `LowPower` animation ends.

```ini title="rulesmd.ini"
[NANRCT] ; example rules section
PoweredSpecial=yes
```
