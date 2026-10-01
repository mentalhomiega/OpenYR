---
key: ShakeXhi
summary: "The largest sideways screen shake, in pixels, the warhead's detonation starts."
see_also: [ShakeXlo, ShakeYlo, ShakeYhi, "system:warheads"]
when_omitted:
  kind: value
  value: "0"
---

The highest value of the random sideways [screen shake](/systems/warheads/#what-one-blast-does-besides-damage) a projectile with this warhead starts when it detonates. Positive values move the view right or down first, negative ones left or up.

```ini title="rulesmd.ini"
[MyBigBang] ; example Warhead
ShakeXlo=10
ShakeXhi=10
```
