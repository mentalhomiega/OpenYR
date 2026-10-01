---
key: ShakeXlo
summary: "The smallest sideways screen shake, in pixels, the warhead's detonation starts."
see_also: [ShakeXhi, ShakeYlo, ShakeYhi, "system:warheads"]
when_omitted:
  kind: value
  value: "0"
---

The lowest value of the random sideways [screen shake](/systems/warheads/#what-one-blast-does-besides-damage) a projectile with this warhead starts when it detonates. Positive values move the view right or down first, negative ones left or up.

```ini title="rulesmd.ini"
[MyBigBang] ; example Warhead
ShakeXlo=10
ShakeXhi=10
```
